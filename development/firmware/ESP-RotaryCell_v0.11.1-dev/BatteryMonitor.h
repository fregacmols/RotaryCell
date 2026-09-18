#pragma once
#include <Arduino.h>

class BatteryMonitor {
public:
  void begin();
  void update();

  uint32_t millivolts() const;
  float volts() const;
  uint8_t estimatedPercent() const;
  bool readingValid() const;

private:
  uint32_t readBatteryMillivolts() const;
  uint8_t estimatePercent(uint32_t millivolts) const;

  uint32_t _millivolts = 0;
  uint32_t _lastSampleAt = 0;
  bool _valid = false;
};
