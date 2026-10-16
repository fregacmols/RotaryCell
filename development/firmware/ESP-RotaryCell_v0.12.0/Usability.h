#pragma once
#include <Arduino.h>
#include <Preferences.h>
#include "ClockManager.h"

struct UserSettings {
  uint32_t version = 1;
  char ssid[33] = "ESP-RotaryCell";
  char password[64] = "rotarycell";
  uint8_t incomingMode = 0, outgoingMode = 0; // off, allow list, block list
  uint8_t privatePolicy = 0, unknownPolicy = 0; // follow, allow, block
  char incomingList[1201] = "", outgoingList[1201] = "";
  bool block911 = true, blockPremium = true, rejectShort = true;
  uint8_t minimumDigits = 7;
  bool scheduledDnd = false, manualDnd = false;
  uint16_t dndStart = 1320, dndEnd = 360;
  char timezone[65] = "EST5EDT,M3.2.0/2,M11.1.0/2";
  bool powerSaving = true;
  uint16_t pollMs = 500;
};

struct CallRecord {
  uint32_t sequence = 0;
  int64_t utc = 0;
  char number[32] = "";
  char identity[16] = "unknown";
  char outcome[40] = "";
  bool incoming = false, connected = false, silenced = false;
  uint32_t checksum = 0;
};

class Usability {
public:
  UserSettings settings;
  void begin(ClockManager& clock);
  bool save(const UserSettings& candidate);
  bool resetWifi();
  bool dndActive() const;
  bool allowed(bool incoming, const String& number, const String& identity, String& reason) const;
  void startCall(bool incoming, const String& number, const String& identity);
  void identify(const String& number, const String& identity);
  void markConnected();
  void markSilenced() { if(_pending) _current.silenced=true; }
  bool wasConnected() const { return _pending && _current.connected; }
  void finishCall(const String& outcome);
  String historyJson() const;
  bool clearHistory();
  bool historyReady() const { return _historyReady; }
  bool callPending() const { return _pending; }
  static String normalize(const String& number);
  static String quote(const String& value);
private:
  Preferences _prefs;
  ClockManager* _clock = nullptr;
  bool _prefsReady = false, _historyReady = false, _pending = false;
  uint32_t _sequence = 0;
  CallRecord _current;
};
extern Usability usability;
