#include "Console.h"
#include "Config.h"
#include "PhoneLine.h"
#include "RingGenerator.h"
#include "BatteryMonitor.h"
#include "Modem.h"

#include "LogSerial.h"
Console::Console(PhoneLine& phoneLine,
                 RingGenerator& ringGenerator,
                 BatteryMonitor& batteryMonitor,
                 Modem& modem)
  : _phoneLine(phoneLine),
    _ringGenerator(ringGenerator),
    _batteryMonitor(batteryMonitor),
    _modem(modem) {}

void Console::begin() {
  LogSerial.println();
  LogSerial.println("ESP-RotaryCell v0.10.4");
  LogSerial.println("Rotary cellular firmware with maintenance Wi-Fi and persistent diagnostics");
  printHelp();
}

void Console::update() {
  while (Serial.available()) {
    const char incoming = static_cast<char>(Serial.read());

    if (incoming == '\r' || incoming == '\n') {
      if (_usbInputBuffer.length() == 0) continue;

      String line = _usbInputBuffer;
      _usbInputBuffer = "";
      line.trim();

      if (_usbAtTerminalMode) handleATLine(line, _usbAtTerminalMode, "USB");
      else handleNormalLine(line, _usbAtTerminalMode, "USB");
      continue;
    }

    if (_usbInputBuffer.length() < 160) _usbInputBuffer += incoming;
  }
}

void Console::submitWebLine(String line) {
  line.trim();
  if (line.length() == 0) return;

  if (_webAtTerminalMode) handleATLine(line, _webAtTerminalMode, "WEB");
  else handleNormalLine(line, _webAtTerminalMode, "WEB");
}

void Console::resetWebSession() {
  _webAtTerminalMode = false;
}

void Console::handleNormalLine(String line, bool& atTerminalMode, const char* sourceName) {
  line.trim();
  line.toUpperCase();

  if (line.length() == 0) return;

  // Commands are intentionally exact, not first-character matches. This keeps
  // accidental text from being interpreted as a maintenance command.
  if (line == "?") {
    printHelp();
  } else if (line == "S") {
    printStatus();
  } else if (line == "M") {
    _modem.requestStatus();
    printModemStatus();
  } else if (line == "A") {
    atTerminalMode = true;
    LogSerial.println();
    LogSerial.print(sourceName);
    LogSerial.println(" AT TERMINAL MODE");
    LogSerial.println("Enter AT commands normally. Enter EXIT to leave.");
  } else if (line == "W") {
    _modem.pulsePowerKey();
    LogSerial.println("MODEM: wait several seconds, then use M");
  } else if (line == "P") {
    _phoneLine.setVerbosePulses(!_phoneLine.verbosePulses());
    LogSerial.print("VERBOSE PULSE MODE: ");
    LogSerial.println(_phoneLine.verbosePulses() ? "ON" : "OFF");
  } else if (line == "R") {
    if (_phoneLine.isOffHook()) LogSerial.println("RING REFUSED: replace handset first");
    else _ringGenerator.startTest();
  } else if (line == "C") {
    if (_phoneLine.isOffHook()) LogSerial.println("RING REFUSED: replace handset first");
    else _ringGenerator.startCadence();
  } else if (line == "X") {
    _ringGenerator.stop();
    LogSerial.println("RING: stopped");
  } else {
    LogSerial.print("UNKNOWN COMMAND: ");
    LogSerial.println(line);
    LogSerial.println("Type ? for help. Use A before entering AT commands.");
  }
}

void Console::handleATLine(String line, bool& atTerminalMode, const char* sourceName) {
  if (line.equalsIgnoreCase("EXIT")) {
    atTerminalMode = false;
    LogSerial.print(sourceName);
    LogSerial.println(" AT terminal closed");
    return;
  }

  LogSerial.print("[");
  LogSerial.print(sourceName);
  LogSerial.print(" AT>] ");
  LogSerial.println(line);
  _modem.sendRaw(line);
}

void Console::printHelp() const {
  LogSerial.println();
  LogSerial.println("Commands (press Send after each command):");
  LogSerial.println("  ?  Help");
  LogSerial.println("  S  Complete status");
  LogSerial.println("  M  Refresh and display modem status");
  LogSerial.println("  A  Enter AT terminal mode; type EXIT to leave");
  LogSerial.println("  W  Pulse modem PWRKEY if it is not responding");
  LogSerial.println("  P  Toggle verbose rotary-pulse display");
  LogSerial.println("  R  One-second ring test");
  LogSerial.println("  C  One complete manual ring cadence");
  LogSerial.println("  X  Stop physical ringing");
  LogSerial.println();
}

void Console::printStatus() const {
  LogSerial.println();
  LogSerial.println("--- ESP-RotaryCell status ---");

  LogSerial.print("Handset: ");
  LogSerial.println(_phoneLine.isOffHook() ? "OFF HOOK" : "ON HOOK");

  LogSerial.print("Pending dial pulses: ");
  LogSerial.println(_phoneLine.pendingPulseCount());

  LogSerial.print("Ring generator: ");
  LogSerial.println(_ringGenerator.modeName());

  LogSerial.print("Battery: ");
  if (_batteryMonitor.readingValid()) {
    LogSerial.print(_batteryMonitor.volts(), 3);
    LogSerial.print(" V (approximately ");
    LogSerial.print(_batteryMonitor.estimatedPercent());
    LogSerial.println("%)");
  } else {
    LogSerial.print("reading unavailable (");
    LogSerial.print(_batteryMonitor.millivolts());
    LogSerial.println(" mV)");
  }

  printModemStatus();
  LogSerial.println("-----------------------------");
  LogSerial.println();
}

void Console::printModemStatus() const {
  LogSerial.println();
  LogSerial.println("Modem:");

  LogSerial.print("  UART response: ");
  LogSerial.println(_modem.isOnline() ? "ONLINE" : "NO RESPONSE");

  LogSerial.print("  SIM: ");
  LogSerial.println(_modem.simStatus());

  LogSerial.print("  Registration: ");
  LogSerial.println(_modem.registrationText());

  LogSerial.print("  Operator: ");
  LogSerial.println(_modem.operatorName().length() > 0
                   ? _modem.operatorName()
                   : String("UNKNOWN"));

  LogSerial.print("  CSQ RSSI: ");
  if (_modem.rssi() < 0 || _modem.rssi() == 99) {
    LogSerial.println("UNKNOWN");
  } else {
    LogSerial.print(_modem.rssi());
    LogSerial.println(" / 31");
  }

  LogSerial.print("  Incoming call: ");
  LogSerial.println(_modem.isIncomingCall() ? "YES" : "NO");

  LogSerial.print("  Cellular call active: ");
  LogSerial.println(_modem.isCallActive() ? "YES" : "NO");

  LogSerial.print("  Caller ID: ");
  LogSerial.println(_modem.callerId().length() > 0
                   ? _modem.callerId()
                   : String("UNKNOWN"));

  LogSerial.print("  Hardware RI pin: ");
  LogSerial.println(_modem.ringIndicatorActive() ? "ACTIVE" : "INACTIVE");
}
