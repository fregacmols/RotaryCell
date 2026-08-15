#include "PhoneLine.h"
#include "Config.h"

#include "LogSerial.h"
bool PhoneLine::readOffHook() const {
  const bool pinHigh = digitalRead(PIN_SHK) == HIGH;
  return SHK_ACTIVE_HIGH ? pinHigh : !pinHigh;
}

void PhoneLine::begin() {
  pinMode(PIN_SHK, INPUT_PULLUP);

  _rawOffHook = readOffHook();
  _lastRawOffHook = _rawOffHook;
  _stableOffHook = _rawOffHook;
  _reportedOffHook = _stableOffHook;
  _rawChangedAt = millis();

  LogSerial.print("PHONE LINE: initial state = ");
  LogSerial.println(_reportedOffHook ? "OFF HOOK" : "ON HOOK");
}

void PhoneLine::update() {
  const uint32_t now = millis();
  _rawOffHook = readOffHook();

  if (_rawOffHook != _lastRawOffHook) {
    _lastRawOffHook = _rawOffHook;
    _rawChangedAt = now;
  }

  if (_rawOffHook != _stableOffHook &&
      now - _rawChangedAt >= SHK_DEBOUNCE_MS) {
    _stableOffHook = _rawOffHook;
    processStableTransition(_stableOffHook, now);
  }

  if (_awaitingPulseReturn &&
      _reportedOffHook &&
      !_stableOffHook &&
      now - _breakStartedAt >= HANGUP_CONFIRM_MS) {
    _awaitingPulseReturn = false;
    _pulseCount = 0;
    _reportedOffHook = false;
    LogSerial.println("HANDSET: ON HOOK");
  }

  if (_pulseCount > 0 &&
      _reportedOffHook &&
      _stableOffHook &&
      !_awaitingPulseReturn &&
      now - _lastPulseAt >= DIGIT_COMPLETE_MS) {
    finishDigit();
  }
}

void PhoneLine::processStableTransition(bool newOffHook, uint32_t now) {
  if (!newOffHook) {
    if (_reportedOffHook) {
      _breakStartedAt = now;
      _awaitingPulseReturn = true;
    }
    return;
  }

  if (_awaitingPulseReturn && _reportedOffHook) {
    const uint32_t breakLength = now - _breakStartedAt;
    _awaitingPulseReturn = false;

    if (breakLength >= DIAL_PULSE_MIN_MS &&
        breakLength <= DIAL_PULSE_MAX_MS) {
      if (_pulseCount < 10) {
        ++_pulseCount;
      }
      _lastPulseAt = now;

      if (_verbosePulses) {
        LogSerial.print("PULSE: ");
        LogSerial.print(_pulseCount);
        LogSerial.print("  break=");
        LogSerial.print(breakLength);
        LogSerial.println(" ms");
      }
    } else if (_verbosePulses) {
      LogSerial.print("IGNORED BREAK: ");
      LogSerial.print(breakLength);
      LogSerial.println(" ms");
    }
    return;
  }

  if (!_reportedOffHook) {
    _reportedOffHook = true;
    _pulseCount = 0;
    LogSerial.println("HANDSET: OFF HOOK");
  }
}

void PhoneLine::finishDigit() {
  const uint8_t digit = (_pulseCount == 10) ? 0 : _pulseCount;
  _completedDigit = digit;
  _digitReady = true;
  LogSerial.print("DIGIT: ");
  LogSerial.println(digit);
  _pulseCount = 0;
}

bool PhoneLine::takeCompletedDigit(uint8_t& digit) {
  if (!_digitReady) {
    return false;
  }

  digit = _completedDigit;
  _digitReady = false;
  return true;
}

bool PhoneLine::isOffHook() const {
  return _reportedOffHook;
}

bool PhoneLine::rawOffHook() const {
  return _rawOffHook;
}

uint8_t PhoneLine::pendingPulseCount() const {
  return _pulseCount;
}

void PhoneLine::setVerbosePulses(bool enabled) {
  _verbosePulses = enabled;
}

bool PhoneLine::verbosePulses() const {
  return _verbosePulses;
}
