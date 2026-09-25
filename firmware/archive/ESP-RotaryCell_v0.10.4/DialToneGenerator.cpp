#include "DialToneGenerator.h"
#include "Config.h"
#include <esp_timer.h>
#include <math.h>

#if __has_include(<esp_arduino_version.h>)
#include <esp_arduino_version.h>
#endif
#include "LogSerial.h"

namespace {
constexpr uint32_t PWM_FREQUENCY_HZ = 31250;
constexpr uint8_t PWM_RESOLUTION_BITS = 8;
constexpr uint8_t PWM_IDLE_DUTY = 127;
constexpr uint8_t PWM_CHANNEL = 0; // Used only with Arduino-ESP32 2.x.
}

void DialToneGenerator::begin() {
  // GPIO36 is a camera data pin on the LilyGO S3-Standard layout. The camera
  // interface is unused in ESP-RotaryCell, so it is available for dial tone.
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcAttach(PIN_DIAL_TONE, PWM_FREQUENCY_HZ, PWM_RESOLUTION_BITS);
  ledcWrite(PIN_DIAL_TONE, PWM_IDLE_DUTY);
#else
  ledcSetup(PWM_CHANNEL, PWM_FREQUENCY_HZ, PWM_RESOLUTION_BITS);
  ledcAttachPin(PIN_DIAL_TONE, PWM_CHANNEL);
  ledcWrite(PWM_CHANNEL, PWM_IDLE_DUTY);
#endif

  // Keep ordinary call-progress tones at the proven 4 kHz update rate so the
  // timer does not add unnecessary load while rotary digits are being polled.
  for (uint16_t i = 0; i < CALL_PROGRESS_TABLE_LENGTH; ++i) {
    const float t = static_cast<float>(i) / CALL_PROGRESS_SAMPLE_RATE_HZ;

    const float dial = sinf(2.0f * PI * 350.0f * t) +
                       sinf(2.0f * PI * 440.0f * t);
    _dialSamples[i] = static_cast<uint8_t>(constrain(
      static_cast<int>(127.0f + 48.0f * dial), 0, 255));

    const float reorder = sinf(2.0f * PI * 480.0f * t) +
                          sinf(2.0f * PI * 620.0f * t);
    _reorderSamples[i] = static_cast<uint8_t>(constrain(
      static_cast<int>(127.0f + 48.0f * reorder), 0, 255));

  }

  // The receiver-off-hook warning uses 8 kHz so its 2600 Hz component remains
  // below Nyquist.  Its 100 ms table contains exact cycles of all four tones.
  for (uint16_t i = 0; i < HOWLER_TABLE_LENGTH; ++i) {
    const float t = static_cast<float>(i) / HOWLER_SAMPLE_RATE_HZ;
    const float howler = sinf(2.0f * PI * 1400.0f * t) +
                         sinf(2.0f * PI * 2060.0f * t) +
                         sinf(2.0f * PI * 2450.0f * t) +
                         sinf(2.0f * PI * 2600.0f * t);
    _howlerSamples[i] = static_cast<uint8_t>(constrain(
      static_cast<int>(127.0f + 24.0f * howler), 0, 255));
  }

  esp_timer_create_args_t args{};
  args.callback = &DialToneGenerator::timerThunk;
  args.arg = this;
  args.dispatch_method = ESP_TIMER_TASK;
  args.name = "dialtone";

  esp_timer_handle_t handle = nullptr;
  if (esp_timer_create(&args, &handle) == ESP_OK) {
    _timer = handle;
  } else {
    LogSerial.println("DIAL TONE: timer creation failed");
  }
}

void DialToneGenerator::startDialTone() {
  startMode(Mode::DIAL);
}

void DialToneGenerator::startReorderTone() {
  startMode(Mode::REORDER);
}

void DialToneGenerator::startHowlerTone() {
  startMode(Mode::HOWLER);
}

void DialToneGenerator::startMode(Mode mode) {
  if (_timer == nullptr) {
    return;
  }

  if (_active) {
    esp_timer_stop(static_cast<esp_timer_handle_t>(_timer));
  }

  _sampleIndex = 0;
  _cadenceIndex = 0;
  _mode = mode;
  _active = true;
  if (mode == Mode::DIAL) writeDuty(_dialSamples[0]);
  else if (mode == Mode::REORDER) writeDuty(_reorderSamples[0]);
  else if (mode == Mode::HOWLER) writeDuty(_howlerSamples[0]);
  else writeDuty(PWM_IDLE_DUTY);

  const uint32_t sampleRate = (mode == Mode::HOWLER)
                                ? HOWLER_SAMPLE_RATE_HZ
                                : CALL_PROGRESS_SAMPLE_RATE_HZ;
  const uint64_t intervalUs = 1000000ULL / sampleRate;
  if (esp_timer_start_periodic(static_cast<esp_timer_handle_t>(_timer), intervalUs) != ESP_OK) {
    _active = false;
    _mode = Mode::OFF;
    writeDuty(PWM_IDLE_DUTY);
    LogSerial.println("CALL PROGRESS TONE: timer start failed");
    return;
  }

  if (mode == Mode::DIAL) {
    LogSerial.println("DIAL TONE: ON (350 + 440 Hz)");
  } else if (mode == Mode::REORDER) {
    LogSerial.println("REORDER TONE: ON (480 + 620 Hz, 250 ms cadence)");
  } else if (mode == Mode::HOWLER) {
    LogSerial.println("HOWLER: ON (1400 + 2060 + 2450 + 2600 Hz, 100 ms cadence)");
  }
}

void DialToneGenerator::stop() {
  if (!_active) {
    return;
  }

  _active = false;
  if (_timer != nullptr) {
    esp_timer_stop(static_cast<esp_timer_handle_t>(_timer));
  }
  const Mode previousMode = _mode;
  _mode = Mode::OFF;
  writeDuty(PWM_IDLE_DUTY);
  if (previousMode == Mode::DIAL) LogSerial.println("DIAL TONE: OFF");
  else if (previousMode == Mode::REORDER) LogSerial.println("REORDER TONE: OFF");
  else if (previousMode == Mode::HOWLER) LogSerial.println("HOWLER: OFF");
}

bool DialToneGenerator::isActive() const {
  return _active;
}

bool DialToneGenerator::isDialToneActive() const {
  return _active && _mode == Mode::DIAL;
}

DialToneGenerator::Mode DialToneGenerator::mode() const {
  return _mode;
}

void DialToneGenerator::timerThunk(void* arg) {
  static_cast<DialToneGenerator*>(arg)->onSample();
}

void DialToneGenerator::onSample() {
  if (!_active) {
    return;
  }

  const Mode currentMode = _mode;
  bool toneOn = true;
  const uint8_t* samples = _dialSamples;

  if (currentMode == Mode::REORDER) {
    samples = _reorderSamples;
    toneOn = _cadenceIndex < REORDER_ON_SAMPLES;
  } else if (currentMode == Mode::HOWLER) {
    samples = _howlerSamples;
    toneOn = _cadenceIndex < HOWLER_ON_SAMPLES;
  } else if (currentMode != Mode::DIAL) {
    toneOn = false;
  }

  writeDuty(toneOn ? samples[_sampleIndex] : PWM_IDLE_DUTY);
  ++_sampleIndex;
  const uint16_t tableLength = (currentMode == Mode::HOWLER)
                                 ? HOWLER_TABLE_LENGTH
                                 : CALL_PROGRESS_TABLE_LENGTH;
  if (_sampleIndex >= tableLength) {
    _sampleIndex = 0;
  }

  if (currentMode == Mode::REORDER) {
    ++_cadenceIndex;
    if (_cadenceIndex >= REORDER_PERIOD_SAMPLES) _cadenceIndex = 0;
  } else if (currentMode == Mode::HOWLER) {
    ++_cadenceIndex;
    if (_cadenceIndex >= HOWLER_PERIOD_SAMPLES) _cadenceIndex = 0;
  }
}

void DialToneGenerator::writeDuty(uint8_t duty) {
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcWrite(PIN_DIAL_TONE, duty);
#else
  ledcWrite(PWM_CHANNEL, duty);
#endif
}
