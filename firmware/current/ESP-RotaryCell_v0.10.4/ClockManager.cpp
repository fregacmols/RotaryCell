#include "ClockManager.h"

void ClockManager::begin() {
  _baseMillis = millis();
}

void ClockManager::setTime(time_t utcEpoch, int timezoneOffsetMinutes, const char* source) {
  _baseUtc = utcEpoch;
  _tzOffsetMinutes = timezoneOffsetMinutes;
  _baseMillis = millis();
  _synced = true;
  _source = source ? source : "UNKNOWN";
}

void ClockManager::setNetworkTime(time_t utcEpoch, int timezoneQuarterHours) {
  setTime(utcEpoch, timezoneQuarterHours * 15, "CELLULAR");
}

void ClockManager::setBrowserTime(time_t utcEpoch, int timezoneOffsetMinutes) {
  setTime(utcEpoch, timezoneOffsetMinutes, "BROWSER");
}

bool ClockManager::isSynced() const {
  return _synced;
}

time_t ClockManager::currentUtc() const {
  if (!_synced) return 0;
  return _baseUtc + static_cast<time_t>((millis() - _baseMillis) / 1000UL);
}

uint32_t ClockManager::secondsSinceSync() const {
  return _synced ? (millis() - _baseMillis) / 1000UL : 0;
}

int ClockManager::timezoneOffsetMinutes() const {
  return _tzOffsetMinutes;
}

String ClockManager::sourceName() const {
  return _source;
}

String ClockManager::timestamp() const {
  if (!_synced) {
    const uint32_t total = millis() / 1000UL;
    const uint32_t h = total / 3600UL;
    const uint8_t m = (total / 60UL) % 60UL;
    const uint8_t s = total % 60UL;
    char buffer[24];
    snprintf(buffer, sizeof(buffer), "[+%lu:%02u:%02u]", static_cast<unsigned long>(h), m, s);
    return String(buffer);
  }

  time_t localEpoch = currentUtc() + static_cast<time_t>(_tzOffsetMinutes) * 60;
  struct tm tmValue;
  gmtime_r(&localEpoch, &tmValue);
  char buffer[28];
  strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S]", &tmValue);
  return String(buffer);
}

String ClockManager::displayTime() const {
  String value = timestamp();
  if (value.startsWith("[") && value.endsWith("]")) {
    value.remove(value.length() - 1);
    value.remove(0, 1);
  }
  return value;
}
