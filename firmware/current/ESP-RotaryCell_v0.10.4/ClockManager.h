#pragma once
#include <Arduino.h>
#include <time.h>

class ClockManager {
public:
  void begin();
  void setNetworkTime(time_t utcEpoch, int timezoneQuarterHours);
  void setBrowserTime(time_t utcEpoch, int timezoneOffsetMinutes);
  bool isSynced() const;
  String timestamp() const;
  String displayTime() const;
  String sourceName() const;
  uint32_t secondsSinceSync() const;
  int timezoneOffsetMinutes() const;
  time_t currentUtc() const;

private:
  void setTime(time_t utcEpoch, int timezoneOffsetMinutes, const char* source);

  time_t _baseUtc = 0;
  uint32_t _baseMillis = 0;
  int _tzOffsetMinutes = 0;
  bool _synced = false;
  String _source = "UPTIME";
};
