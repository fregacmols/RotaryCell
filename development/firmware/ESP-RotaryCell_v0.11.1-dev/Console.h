#pragma once
#include <Arduino.h>

class PhoneLine;
class RingGenerator;
class BatteryMonitor;
class Modem;

class Console {
public:
  Console(PhoneLine& phoneLine,
          RingGenerator& ringGenerator,
          BatteryMonitor& batteryMonitor,
          Modem& modem);

  bool atTerminalActive() const { return _usbAtTerminalMode || _webAtTerminalMode; }
  bool takeWifiRequest() { bool pending = _wifiRequested; _wifiRequested = false; return pending; }
  void begin();
  void update();
  void submitWebLine(String line);
  void resetWebSession();

private:
  void printHelp() const;
  void printStatus() const;
  void printModemStatus() const;
  void handleNormalLine(String line, bool& atTerminalMode, const char* sourceName);
  void handleATLine(String line, bool& atTerminalMode, const char* sourceName);

  PhoneLine& _phoneLine;
  RingGenerator& _ringGenerator;
  BatteryMonitor& _batteryMonitor;
  Modem& _modem;

  String _usbInputBuffer;
  bool _wifiRequested = false;
  bool _usbAtTerminalMode = false;
  bool _webAtTerminalMode = false;
};
