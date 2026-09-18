#pragma once
#include <Arduino.h>
#include <WebServer.h>

class PhoneLine;
class BatteryMonitor;
class Modem;
class Console;
class EventLog;
class ClockManager;
class HardwareControl;
class PowerLogger;

class MaintenanceService {
public:
  MaintenanceService(PhoneLine& phoneLine,
                     BatteryMonitor& batteryMonitor,
                     Modem& modem,
                     Console& console,
                     EventLog& eventLog,
                     ClockManager& clock,
                     HardwareControl& hardwareControl,
                     PowerLogger& powerLogger);

  void begin();
  void update();
  void toggle();
  void enable();
  void disable();
  bool isEnabled() const;
  uint32_t secondsRemaining() const;
  void setPhoneState(const char* stateName);

private:
  PhoneLine& _phoneLine;
  BatteryMonitor& _batteryMonitor;
  Modem& _modem;
  Console& _console;
  EventLog& _eventLog;
  ClockManager& _clock;
  HardwareControl& _hardwareControl;
  PowerLogger& _powerLogger;
  WebServer _server;
  bool _enabled = false;
  uint32_t _enabledAt = 0;
  String _phoneState = "UNKNOWN";
  bool _otaUploadOk = false;

  void configureRoutes();
  void handleRoot();
  void handleStatus();
  void handleLog();
  void handleEvents();
  void handleCommand();
  void handleTimeSync();
  void handleClearRamLog();
  void handleClearEvents();
  void handleWifiOff();
  void handlePowerSavingToggle();
  void handleOtaUpload();
  void handleOtaComplete();
  String statusJson() const;
  static String jsonEscape(const String& value);
};
