#pragma once
#include <Arduino.h>

class DialToneGenerator {
public:
  enum class Mode : uint8_t {
    OFF,
    DIAL,
    REORDER,
    HOWLER
  };

  void begin();
  void startDialTone();
  void startReorderTone();
  void startHowlerTone();
  void stop();
  bool isActive() const;
  bool isDialToneActive() const;
  Mode mode() const;

private:
  static void timerThunk(void* arg);
  void startMode(Mode mode);
  void onSample();
  void writeDuty(uint8_t duty);

  static constexpr uint16_t CALL_PROGRESS_SAMPLE_RATE_HZ = 4000;
  static constexpr uint16_t HOWLER_SAMPLE_RATE_HZ = 8000;
  static constexpr uint16_t CALL_PROGRESS_TABLE_LENGTH = 400; // 100 ms
  static constexpr uint16_t HOWLER_TABLE_LENGTH = 800;        // 100 ms
  static constexpr uint16_t REORDER_ON_SAMPLES = CALL_PROGRESS_SAMPLE_RATE_HZ / 4;
  static constexpr uint16_t REORDER_PERIOD_SAMPLES = CALL_PROGRESS_SAMPLE_RATE_HZ / 2;
  static constexpr uint16_t HOWLER_ON_SAMPLES = HOWLER_SAMPLE_RATE_HZ / 10;
  static constexpr uint16_t HOWLER_PERIOD_SAMPLES = HOWLER_SAMPLE_RATE_HZ / 5;

  uint8_t _dialSamples[CALL_PROGRESS_TABLE_LENGTH]{};
  uint8_t _reorderSamples[CALL_PROGRESS_TABLE_LENGTH]{};
  uint8_t _howlerSamples[HOWLER_TABLE_LENGTH]{};
  volatile uint16_t _sampleIndex = 0;
  volatile uint16_t _cadenceIndex = 0;
  volatile bool _active = false;
  volatile Mode _mode = Mode::OFF;
  void* _timer = nullptr;
};
