#pragma once
#include <Arduino.h>

class ClockManager;

class EventLog {
public:
  explicit EventLog(ClockManager& clock);
  bool begin();
  void add(const String& eventText);
  void addMultiline(const String& text);
  String contents() const;
  void clear();
  bool ready() const;

private:
  ClockManager& _clock;
  bool _ready = false;
  static constexpr size_t MAX_FILE_SIZE = 65536;
  static constexpr const char* PATH = "/events.log";
};
