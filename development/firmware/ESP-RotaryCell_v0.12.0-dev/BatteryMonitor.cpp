#include "BatteryMonitor.h"
#include "Config.h"

void BatteryMonitor::begin() {
  analogReadResolution(12);
  analogSetPinAttenuation(PIN_BATTERY_ADC, ADC_11db);

  _millivolts = readBatteryMillivolts();
  _lastSampleAt = millis();
  _valid = (_millivolts >= BATTERY_VALID_MIN_MV &&
            _millivolts <= BATTERY_VALID_MAX_MV);
}

void BatteryMonitor::update() {
  const uint32_t now = millis();
  if (now - _lastSampleAt < BATTERY_SAMPLE_INTERVAL_MS) {
    return;
  }

  _lastSampleAt = now;

  uint64_t total = 0;
  for (uint8_t i = 0; i < BATTERY_SAMPLE_COUNT; ++i) {
    total += readBatteryMillivolts();
    delay(2);
  }

  _millivolts = static_cast<uint32_t>(total / BATTERY_SAMPLE_COUNT);
  _valid = (_millivolts >= BATTERY_VALID_MIN_MV &&
            _millivolts <= BATTERY_VALID_MAX_MV);
}

uint32_t BatteryMonitor::readBatteryMillivolts() const {
  const uint32_t adcMillivolts = analogReadMilliVolts(PIN_BATTERY_ADC);
  return adcMillivolts * BATTERY_DIVIDER_RATIO;
}

uint32_t BatteryMonitor::millivolts() const {
  return _millivolts;
}

float BatteryMonitor::volts() const {
  return static_cast<float>(_millivolts) / 1000.0f;
}

bool BatteryMonitor::readingValid() const {
  return _valid;
}

uint8_t BatteryMonitor::estimatedPercent() const {
  if (!_valid) {
    return 0;
  }
  return estimatePercent(_millivolts);
}

uint8_t BatteryMonitor::estimatePercent(uint32_t mv) const {
  struct Point {
    uint16_t mv;
    uint8_t percent;
  };

  static constexpr Point curve[] = {
    {3300,   0},
    {3500,   3},
    {3600,   8},
    {3700,  15},
    {3750,  25},
    {3800,  35},
    {3850,  50},
    {3900,  60},
    {3950,  70},
    {4000,  78},
    {4050,  85},
    {4100,  92},
    {4150,  97},
    {4200, 100}
  };

  if (mv <= curve[0].mv) {
    return curve[0].percent;
  }

  const size_t count = sizeof(curve) / sizeof(curve[0]);
  if (mv >= curve[count - 1].mv) {
    return curve[count - 1].percent;
  }

  for (size_t i = 1; i < count; ++i) {
    if (mv <= curve[i].mv) {
      const uint32_t x0 = curve[i - 1].mv;
      const uint32_t x1 = curve[i].mv;
      const uint32_t y0 = curve[i - 1].percent;
      const uint32_t y1 = curve[i].percent;
      return static_cast<uint8_t>(
        y0 + ((mv - x0) * (y1 - y0)) / (x1 - x0)
      );
    }
  }

  return 0;
}
