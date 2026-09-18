#include "Modem.h"
#include "Config.h"

#include "LogSerial.h"
void Modem::begin() {
  LogSerial.println();
  LogSerial.println("MODEM: starting A7670G interface");

  // LilyGO documents GPIO42 HIGH as maximum power capability.
  pinMode(PIN_MODEM_POWER_SAVE, OUTPUT);
  digitalWrite(PIN_MODEM_POWER_SAVE, HIGH);

  // Keep the modem awake.
  pinMode(PIN_MODEM_DTR, OUTPUT);
  digitalWrite(PIN_MODEM_DTR, LOW);

  pinMode(PIN_MODEM_RING, INPUT_PULLUP);

  // LilyGO A7670G-S3 PWRKEY sequence idles LOW and uses a HIGH pulse.
  pinMode(PIN_MODEM_PWRKEY, OUTPUT);
  digitalWrite(PIN_MODEM_PWRKEY, LOW);

  _serial.begin(MODEM_BAUD, SERIAL_8N1, PIN_MODEM_RX, PIN_MODEM_TX);
  delay(150);

  LogSerial.println("MODEM: checking for an existing AT response");
  if (!waitForAT(2500)) {
    LogSerial.println("MODEM: no response; issuing a PWRKEY wake pulse");
    pulsePowerKey();

    const uint32_t startedAt = millis();
    while (millis() - startedAt < MODEM_BOOT_WAIT_MS) {
      readSerial();
      delay(10);
    }

    if (!waitForAT(3000)) {
      LogSerial.println("MODEM: still not responding");
      LogSerial.println("MODEM: use command W to retry PWRKEY, or A for AT terminal");
      return;
    }
  }

  _online = true;
  LogSerial.println("MODEM: responding");
  configureModem();
  requestStatus();
}

bool Modem::waitForAT(uint32_t timeoutMs) {
  while (_serial.available()) {
    _serial.read();
  }

  _serial.print("AT\r\n");
  const uint32_t startedAt = millis();
  String response;

  while (millis() - startedAt < timeoutMs) {
    while (_serial.available()) {
      const char c = static_cast<char>(_serial.read());
      response += c;
      if (response.indexOf("OK") >= 0) {
        return true;
      }
    }
    delay(5);
  }

  return false;
}

void Modem::configureModem() {
  // Echo off, verbose errors, caller ID, extended call result codes,
  // and registration URCs.
  sendRaw("ATE0");
  delay(100);
  sendRaw("AT+CMEE=2");
  delay(100);
  sendRaw("AT+CLIP=1");
  delay(100);
  sendRaw("AT+CRC=1");
  delay(100);
  sendRaw("AT+CREG=2");
  delay(100);
  sendRaw("AT+CEREG=2");
  delay(100);
  // Allow the modem to accept network-provided time/timezone (NITZ) when available.
  sendRaw("AT+CTZU=1");
  delay(100);

  // LilyGO T-A7670G-S3 Standard: modem GPIO3 enables the onboard
  // speaker power amplifier.  Also select the speaker audio path and
  // restore the receive gain established during bench testing.
  configureVoiceAudio();
}

void Modem::configureVoiceAudio() {
  LogSerial.println("MODEM: configuring voice audio / speaker amplifier");
  sendRaw("AT+CGDRT=3,1");
  delay(100);
  sendRaw("AT+CGSETV=3,1");
  delay(100);
  sendRaw("AT+CSDVC=3");
  delay(100);
  sendRaw("AT+COUTGAIN=7");
  delay(100);
}

void Modem::pulsePowerKey() {
  LogSerial.println("MODEM: PWRKEY LOW 100 ms, HIGH 1000 ms, then LOW");
  digitalWrite(PIN_MODEM_PWRKEY, LOW);
  delay(MODEM_POWERKEY_LOW_PREP_MS);
  digitalWrite(PIN_MODEM_PWRKEY, HIGH);
  delay(MODEM_POWERKEY_HIGH_MS);
  digitalWrite(PIN_MODEM_PWRKEY, LOW);
}

String Modem::collectCommand(const String& command, uint32_t timeoutMs) {
  while (_serial.available()) _serial.read();
  _lineBuffer = "";

  String out = "> " + command + "\n";
  _serial.print(command);
  _serial.print("\r\n");
  const uint32_t started = millis();
  String line;

  while (millis() - started < timeoutMs) {
    while (_serial.available()) {
      const char c = static_cast<char>(_serial.read());
      if (c == '\r') continue;
      if (c == '\n') {
        line.trim();
        if (line.length()) {
          out += "< " + line + "\n";
          processLine(line);
          if (line == "OK" || line == "ERROR" || line.startsWith("+CME ERROR") || line.startsWith("+CMS ERROR")) {
            return out;
          }
        }
        line = "";
      } else if (line.length() < 255) {
        line += c;
      }
    }
    delay(5);
  }
  if (line.length()) {
    line.trim();
    if (line.length()) { out += "< " + line + "\n"; processLine(line); }
  }
  out += "< [TIMEOUT]\n";
  return out;
}

String Modem::diagnosticSnapshot(const char* label) {
  String report;
  report.reserve(1800);
  report += String("--- ") + label + " MODEM SNAPSHOT ---\n";
  // Standard 3GPP/SIMCom queries chosen for this A7670G build.
  // Keep the snapshot compact enough to preserve several recovery events.
  const char* commands[] = {
    "AT", "AT+CPIN?", "AT+CSQ", "AT+COPS?", "AT+CREG?", "AT+CGREG?",
    "AT+CEREG?", "AT+CLCC", "AT+CGATT?", "AT+CPSI?", "AT+CNMP?",
    "AT+CCLK?"
  };
  for (const char* command : commands) {
    report += collectCommand(command, 1500);
  }
  report += String("RI pin: ") + (ringIndicatorActive() ? "ACTIVE" : "INACTIVE") + "\n";
  report += String("Firmware callActive: ") + (_callActive ? "YES" : "NO") + "\n";
  report += String("Firmware incomingCall: ") + (_incomingCall ? "YES" : "NO") + "\n";
  report += String("--- END ") + label + " MODEM SNAPSHOT ---";
  return report;
}

bool Modem::restartAndRecover() {
  LogSerial.println("MODEM RECOVERY: requesting orderly power-down (AT+CPOF)");
  _serial.print("AT+CPOF\r\n");
  delay(4000);
  _online = false;
  _registered = false;
  _callActive = false;
  setIncomingCall(false);

  LogSerial.println("MODEM RECOVERY: pulsing PWRKEY to start modem");
  pulsePowerKey();
  delay(MODEM_BOOT_WAIT_MS);

  if (!waitForAT(3000)) {
    // If the modem ignored CPOF, the first pulse may have shut it down instead.
    // One second pulse again gives us a conservative second chance to start it.
    LogSerial.println("MODEM RECOVERY: no AT response; trying one additional PWRKEY pulse");
    pulsePowerKey();
    delay(MODEM_BOOT_WAIT_MS);
    if (!waitForAT(3000)) {
      LogSerial.println("MODEM RECOVERY: modem did not return");
      return false;
    }
  }

  _online = true;
  configureModem();
  requestStatus();

  const uint32_t registrationStarted = millis();
  while (millis() - registrationStarted < 30000UL) {
    readSerial();
    if (_registered) return true;
    if ((millis() - registrationStarted) % 5000UL < 20) {
      sendRaw("AT+CEREG?");
    }
    delay(20);
  }
  return _online;
}

void Modem::update() {
  readSerial();

  const uint32_t now = millis();

  // Incoming-call URCs normally repeat. If they stop without an explicit
  // NO CARRIER, release the ringing state after a generous timeout.
  if (_incomingCall &&
      _lastRingAt > 0 &&
      now - _lastRingAt >= MODEM_RING_TIMEOUT_MS) {
    LogSerial.println("MODEM: ring URC timeout");
    setIncomingCall(false);
  }

  // Status polling is manual only (console command M).
}

void Modem::readSerial() {
  while (_serial.available()) {
    const char c = static_cast<char>(_serial.read());

    if (c == '\r') {
      continue;
    }

    if (c == '\n') {
      if (_lineBuffer.length() > 0) {
        processLine(_lineBuffer);
        _lineBuffer = "";
      }
      continue;
    }

    if (_lineBuffer.length() < 255) {
      _lineBuffer += c;
    } else {
      _lineBuffer = "";
    }
  }
}

void Modem::processLine(String line) {
  line.trim();
  if (line.length() == 0) {
    return;
  }

  LogSerial.print("[MODEM] ");
  LogSerial.println(line);

  if (line == "OK") {
    _online = true;
    return;
  }

  if (line == "RING" || line.startsWith("+CRING:")) {
    _lastRingAt = millis();
    setIncomingCall(true);
    return;
  }

  if (line == "NO CARRIER" ||
      line == "BUSY" ||
      line == "NO ANSWER" ||
      line == "NO DIALTONE") {
    if (line == "BUSY") {
      _callResultEvent = CallResult::BUSY;
    } else if (line == "NO ANSWER") {
      _callResultEvent = CallResult::NO_ANSWER;
    } else if (line == "NO DIALTONE") {
      _callResultEvent = CallResult::NO_DIALTONE;
    } else {
      _callResultEvent = CallResult::NO_CARRIER;
    }

    _callActive = false;
    setIncomingCall(false);
    return;
  }

  if (line.startsWith("+CLIP:")) {
    parseClip(line);
    return;
  }

  if (line.startsWith("+CPIN:")) {
    parseCpin(line);
    return;
  }

  if (line.startsWith("+CSQ:")) {
    parseCsq(line);
    return;
  }

  if (line.startsWith("+COPS:")) {
    parseOperator(line);
    return;
  }

  if (line.startsWith("+CCLK:")) {
    if (_clockRequestPending) {
      _clockRequestPending = false;
      parseClock(line);
    }
    return;
  }

  if (line.startsWith("+CREG:") ||
      line.startsWith("+CEREG:") ||
      line.startsWith("+CGREG:")) {
    parseRegistration(line);
    return;
  }

  if (line.indexOf("ERROR") >= 0) {
    _online = true;
  }
}

void Modem::parseClip(const String& line) {
  const int firstQuote = line.indexOf('"');
  if (firstQuote < 0) {
    return;
  }

  const int secondQuote = line.indexOf('"', firstQuote + 1);
  if (secondQuote <= firstQuote) {
    return;
  }

  _callerId = line.substring(firstQuote + 1, secondQuote);
  LogSerial.print("MODEM: caller ID = ");
  LogSerial.println(_callerId);
}

void Modem::parseCpin(const String& line) {
  const int colon = line.indexOf(':');
  if (colon < 0) {
    return;
  }

  _simStatus = line.substring(colon + 1);
  _simStatus.trim();
  _simReady = (_simStatus == "READY");
}

void Modem::parseCsq(const String& line) {
  const int colon = line.indexOf(':');
  const int comma = line.indexOf(',', colon + 1);
  if (colon < 0 || comma < 0) {
    return;
  }

  _rssi = line.substring(colon + 1, comma).toInt();
}

static String friendlyNetworkName(const String& plmn) {
  // Major U.S. carrier PLMNs. Unknown/MVNO codes remain visible numerically.
  if (plmn == "310260" || plmn == "310160" || plmn == "310200" ||
      plmn == "310210" || plmn == "310220" || plmn == "310230" ||
      plmn == "310240" || plmn == "310250" || plmn == "310270" ||
      plmn == "310310" || plmn == "310490" || plmn == "310660") {
    return "T-Mobile";
  }
  if (plmn == "310410" || plmn == "310380" || plmn == "310150" ||
      plmn == "310170" || plmn == "310280" || plmn == "310030" ||
      plmn == "310070" || plmn == "310090" || plmn == "310560" ||
      plmn == "310680") {
    return "AT&T";
  }
  if (plmn == "311480" || plmn == "310012" || plmn == "311390") {
    return "Verizon";
  }
  if (plmn == "311580" || plmn == "311220") {
    return "US Cellular";
  }
  if (plmn == "313100") {
    return "AT&T / FirstNet";
  }
  return plmn;
}

void Modem::parseOperator(const String& line) {
  const int firstQuote = line.indexOf('"');
  if (firstQuote >= 0) {
    const int secondQuote = line.indexOf('"', firstQuote + 1);
    if (secondQuote > firstQuote) {
      String value = line.substring(firstQuote + 1, secondQuote);
      value.trim();
      _operatorName = friendlyNetworkName(value);
      return;
    }
  }

  _operatorName = line;
}

void Modem::parseRegistration(const String& line) {
  const bool isCereg = line.startsWith("+CEREG:");

  // On LTE the A7670G can report a CS-domain +CREG status such as 7 while
  // +CEREG correctly reports EPS registration as 1/5. Once CEREG has been
  // observed, use it as the dashboard registration source and do not let
  // later CREG/CGREG URCs overwrite it.
  if (isCereg) {
    _seenCereg = true;
  } else if (_seenCereg) {
    return;
  }

  const int colon = line.indexOf(':');
  if (colon < 0) {
    return;
  }

  String values = line.substring(colon + 1);
  values.trim();

  int status = -1;
  const int comma = values.indexOf(',');
  if (comma >= 0) {
    String first = values.substring(0, comma);
    first.trim();

    const int secondComma = values.indexOf(',', comma + 1);
    String second = (secondComma >= 0)
                      ? values.substring(comma + 1, secondComma)
                      : values.substring(comma + 1);
    second.trim();

    // Query replies are typically <n>,<stat>,... while unsolicited URCs are
    // <stat>,<tac/lac>,... . Treat the second field as <stat> only when it
    // is a plain decimal registration value (0-10); otherwise the first
    // field is the registration status.
    bool secondIsDecimal = second.length() > 0;
    for (size_t i = 0; i < second.length(); ++i) {
      if (!isDigit(second[i])) {
        secondIsDecimal = false;
        break;
      }
    }
    const int secondValue = secondIsDecimal ? second.toInt() : -1;
    if (secondIsDecimal && secondValue >= 0 && secondValue <= 10) {
      status = secondValue;
    } else {
      status = first.toInt();
    }
  } else {
    status = values.toInt();
  }

  switch (status) {
    case 0:
      _registrationText = "NOT REGISTERED";
      _registered = false;
      break;
    case 1:
      _registrationText = "REGISTERED (HOME)";
      _registered = true;
      break;
    case 2:
      _registrationText = "SEARCHING";
      _registered = false;
      break;
    case 3:
      _registrationText = "DENIED";
      _registered = false;
      break;
    case 4:
      _registrationText = "UNKNOWN";
      _registered = false;
      break;
    case 5:
      _registrationText = "REGISTERED (ROAMING)";
      _registered = true;
      break;
    default:
      _registrationText = String("UNRECOGNIZED (") + String(status) + ")";
      _registered = false;
      break;
  }
}


static int64_t daysFromCivil(int y, unsigned m, unsigned d) {
  y -= m <= 2;
  const int era = (y >= 0 ? y : y - 399) / 400;
  const unsigned yoe = static_cast<unsigned>(y - era * 400);
  const unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
  const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
  return static_cast<int64_t>(era) * 146097 + static_cast<int64_t>(doe) - 719468;
}

void Modem::parseClock(const String& line) {
  // SIMCom/3GPP format: +CCLK: "yy/MM/dd,hh:mm:ss+zz"
  // zz is the local offset from UTC in 15-minute units.
  const int firstQuote = line.indexOf('"');
  const int secondQuote = line.indexOf('"', firstQuote + 1);
  if (firstQuote < 0 || secondQuote <= firstQuote) return;

  String value = line.substring(firstQuote + 1, secondQuote);
  if (value.length() < 17) return;

  const int yy = value.substring(0, 2).toInt();
  const int month = value.substring(3, 5).toInt();
  const int day = value.substring(6, 8).toInt();
  const int hour = value.substring(9, 11).toInt();
  const int minute = value.substring(12, 14).toInt();
  const int second = value.substring(15, 17).toInt();

  if (month < 1 || month > 12 || day < 1 || day > 31 || hour > 23 || minute > 59 || second > 60) return;

  // A7670 returns a two-digit year. For this project, only accept dates in
  // 2024..2069. This deliberately rejects the modem's unsynchronized default
  // 70/01/01, which would otherwise be misread as 2070-01-01.
  const int year = 2000 + yy;
  if (year < 2024 || year > 2069) return;

  int tzQuarterHours = 0;
  if (value.length() >= 20) {
    const char sign = value.charAt(17);
    const int magnitude = value.substring(18, 20).toInt();
    if (sign == '-') tzQuarterHours = -magnitude;
    else if (sign == '+') tzQuarterHours = magnitude;
  }

  const int64_t localSeconds = daysFromCivil(year, month, day) * 86400LL
                             + static_cast<int64_t>(hour) * 3600LL
                             + static_cast<int64_t>(minute) * 60LL
                             + second;
  const int64_t utcSeconds = localSeconds - static_cast<int64_t>(tzQuarterHours) * 15LL * 60LL;
  if (utcSeconds <= 0) return;

  _networkUtcEpoch = static_cast<time_t>(utcSeconds);
  _networkTimezoneQuarterHours = tzQuarterHours;
  _networkTimeEvent = true;
}

void Modem::setIncomingCall(bool active) {
  if (active == _incomingCall) {
    return;
  }

  _incomingCall = active;

  if (active) {
    _incomingStartedEvent = true;
    _incomingEndedEvent = false;
  } else {
    _incomingEndedEvent = true;
    _incomingStartedEvent = false;
    _lastRingAt = 0;
  }
}

void Modem::answerIncomingCall() {
  if (!_incomingCall) {
    LogSerial.println("MODEM: answer ignored; no incoming call");
    return;
  }

  // Reassert the board-specific audio path before every answered call.
  // This is intentionally repeated even though it is also configured at boot.
  configureVoiceAudio();

  _callResultEvent = CallResult::NONE;
  LogSerial.println("MODEM: answering incoming call (ATA)");
  sendRaw("ATA");
  _incomingCall = false;
  _incomingStartedEvent = false;
  _incomingEndedEvent = false;
  _lastRingAt = 0;
  _callActive = true;
}


void Modem::dialOutgoingCall(const String& number) {
  if (number.length() == 0) {
    LogSerial.println("MODEM: dial ignored; empty number");
    return;
  }

  configureVoiceAudio();

  _callResultEvent = CallResult::NONE;
  LogSerial.print("MODEM: dialing ");
  LogSerial.println(number);
  sendRaw("ATD" + number + ";");
  _callActive = true;
  _incomingCall = false;
  _incomingStartedEvent = false;
  _incomingEndedEvent = false;
  _lastRingAt = 0;
}

void Modem::hangUp() {
  if (!_callActive && !_incomingCall) {
    return;
  }

  LogSerial.println("MODEM: hanging up (AT+CHUP)");
  sendRaw("AT+CHUP");
  _callActive = false;
  setIncomingCall(false);
}

void Modem::requestStatus() {
  if (!_online) {
    LogSerial.println("MODEM: status request sent despite offline state");
  }

  sendRaw("AT+CPIN?");
  sendRaw("AT+CSQ");
  sendRaw("AT+COPS?");
  sendRaw("AT+CREG?");
  sendRaw("AT+CEREG?");
  requestClock();
}

void Modem::requestClock() {
  _clockRequestPending = true;
  sendRaw("AT+CCLK?");
}

void Modem::setClock(time_t utcEpoch, int timezoneOffsetMinutes) {
  // AT+CCLK stores local time and a timezone offset expressed in 15-minute units.
  int tzQuarterHours = timezoneOffsetMinutes / 15;
  if (tzQuarterHours > 96) tzQuarterHours = 96;
  if (tzQuarterHours < -96) tzQuarterHours = -96;

  const time_t localEpoch = utcEpoch + static_cast<time_t>(tzQuarterHours) * 15 * 60;
  struct tm tmValue;
  gmtime_r(&localEpoch, &tmValue);

  char command[48];
  const char sign = tzQuarterHours < 0 ? '-' : '+';
  const int magnitude = abs(tzQuarterHours);
  snprintf(command, sizeof(command),
           "AT+CCLK=\"%02d/%02d/%02d,%02d:%02d:%02d%c%02d\"",
           (tmValue.tm_year + 1900) % 100, tmValue.tm_mon + 1, tmValue.tm_mday,
           tmValue.tm_hour, tmValue.tm_min, tmValue.tm_sec, sign, magnitude);
  sendRaw(String(command));
}

void Modem::sendRaw(const String& command) {
  if (command.length() == 0) {
    return;
  }

  _serial.print(command);
  _serial.print("\r\n");
}

bool Modem::isOnline() const {
  return _online;
}

bool Modem::simReady() const {
  return _simReady;
}

bool Modem::isRegistered() const {
  return _registered;
}

bool Modem::isIncomingCall() const {
  return _incomingCall;
}

bool Modem::isCallActive() const {
  return _callActive;
}

bool Modem::ringIndicatorActive() const {
  // RI is normally active low on SIMCom modules.
  return digitalRead(PIN_MODEM_RING) == LOW;
}

int Modem::rssi() const {
  return _rssi;
}

const String& Modem::operatorName() const {
  return _operatorName;
}

const String& Modem::callerId() const {
  return _callerId;
}

const String& Modem::simStatus() const {
  return _simStatus;
}

const String& Modem::registrationText() const {
  return _registrationText;
}

bool Modem::takeIncomingCallStarted() {
  const bool value = _incomingStartedEvent;
  _incomingStartedEvent = false;
  return value;
}

bool Modem::takeIncomingCallEnded() {
  const bool value = _incomingEndedEvent;
  _incomingEndedEvent = false;
  return value;
}


bool Modem::takeCallResult(CallResult& result) {
  if (_callResultEvent == CallResult::NONE) {
    return false;
  }

  result = _callResultEvent;
  _callResultEvent = CallResult::NONE;
  return true;
}


bool Modem::takeNetworkTime(time_t& utcEpoch, int& timezoneQuarterHours) {
  if (!_networkTimeEvent) return false;
  utcEpoch = _networkUtcEpoch;
  timezoneQuarterHours = _networkTimezoneQuarterHours;
  _networkTimeEvent = false;
  return true;
}

const char* Modem::callResultName(CallResult result) {
  switch (result) {
    case CallResult::NO_CARRIER:  return "NO CARRIER";
    case CallResult::BUSY:        return "BUSY";
    case CallResult::NO_ANSWER:   return "NO ANSWER";
    case CallResult::NO_DIALTONE: return "NO DIALTONE";
    default:                      return "NONE";
  }
}
