#include "EventLog.h"
#include "ClockManager.h"
#include <LittleFS.h>

constexpr const char* EventLog::PATH;

EventLog::EventLog(ClockManager& clock) : _clock(clock) {}

bool EventLog::begin() {
  _ready = LittleFS.begin(true);
  return _ready;
}

void EventLog::add(const String& eventText) {
  if (!_ready) return;

  File existing = LittleFS.open(PATH, "r");
  const size_t existingSize = existing ? existing.size() : 0;
  if (existing) existing.close();

  if (existingSize >= MAX_FILE_SIZE) {
    LittleFS.remove(PATH);
    File rotated = LittleFS.open(PATH, "w");
    if (rotated) {
      rotated.println(_clock.timestamp() + " EVENT LOG ROTATED");
      rotated.close();
    }
  }

  File file = LittleFS.open(PATH, "a");
  if (!file) return;
  file.println(_clock.timestamp() + " " + eventText);
  file.close();
}

void EventLog::addMultiline(const String& text) {
  if (!_ready) return;
  int start = 0;
  while (start <= (int)text.length()) {
    int end = text.indexOf('\n', start);
    if (end < 0) end = text.length();
    String line = text.substring(start, end);
    line.trim();
    if (line.length()) add(line);
    if (end >= (int)text.length()) break;
    start = end + 1;
  }
}

String EventLog::contents() const {
  if (!_ready) return String("Event log unavailable: LittleFS not mounted.\n");
  File file = LittleFS.open(PATH, "r");
  if (!file) return String();
  String result;
  result.reserve(file.size() + 1);
  while (file.available()) {
    result += static_cast<char>(file.read());
  }
  file.close();
  return result;
}

void EventLog::clear() {
  if (!_ready) return;
  LittleFS.remove(PATH);
}

bool EventLog::ready() const { return _ready; }
