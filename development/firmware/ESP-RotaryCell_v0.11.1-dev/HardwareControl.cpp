#include "HardwareControl.h"

#include "Config.h"
#include "LogSerial.h"

void HardwareControl::begin() {
  // PD must be either pulled LOW or left open. Never drive it HIGH.
  releasePowerDownPin();
  _ag1171State = Ag1171State::Awake;
  _stateChangedAt = millis();

  digitalWrite(PIN_HARD_RESET_TRIGGER, LOW);
  pinMode(PIN_HARD_RESET_TRIGGER, OUTPUT);
}
void HardwareControl::update(bool mustStayAwake, bool offHook) {
  const uint32_t now = millis();

  if (!_powerSavingEnabled) {
    if (_ag1171State == Ag1171State::PoweredDown) {
      startWake(now);
    } else if (_ag1171State != Ag1171State::Awake &&
               now - _stateChangedAt >= AG1171_WAKE_TIME_MS) {
      _ag1171State = Ag1171State::Awake;
      _stateChangedAt = now;
      LogSerial.println("AG1171: awake; power saving disabled");
    }
    return;
  }

  if (mustStayAwake || offHook) {
    if (_ag1171State == Ag1171State::PoweredDown) {
      startWake(now);
    } else if (_ag1171State == Ag1171State::Waking &&
               now - _stateChangedAt >= AG1171_WAKE_TIME_MS) {
      _ag1171State = Ag1171State::Awake;
      _stateChangedAt = now;
    } else if (_ag1171State == Ag1171State::PollingHook) {
      _ag1171State = Ag1171State::Awake;
      _stateChangedAt = now;
    }
    return;
  }

  switch (_ag1171State) {
    case Ag1171State::Awake:
      enterPoweredDown(now);
      break;

    case Ag1171State::PoweredDown:
      if (now - _stateChangedAt >= AG1171_HOOK_POLL_INTERVAL_MS) {
        startWake(now);
      }
      break;

    case Ag1171State::Waking:
      if (now - _stateChangedAt >= AG1171_WAKE_TIME_MS) {
        _ag1171State = Ag1171State::PollingHook;
        _stateChangedAt = now;
      }
      break;

    case Ag1171State::PollingHook:
      // PhoneLine receives and debounces SHK while this window is open.
      if (offHook) {
        _ag1171State = Ag1171State::Awake;
        _stateChangedAt = now;
        LogSerial.println("AG1171: handset detected during power-saving poll");
      } else if (now - _stateChangedAt >= AG1171_HOOK_SAMPLE_WINDOW_MS) {
        enterPoweredDown(now);
      }
      break;
  }
}

void HardwareControl::setPowerSavingEnabled(bool enabled) {
  if (_powerSavingEnabled == enabled) {
    return;
  }

  _powerSavingEnabled = enabled;
  LogSerial.print("AG1171: experimental power saving ");
  LogSerial.println(enabled ? "ENABLED" : "DISABLED");

  if (!enabled) {
    requestAg1171Wake();
  }
}

bool HardwareControl::powerSavingEnabled() const {
  return _powerSavingEnabled;
}

bool HardwareControl::hookSignalValid() const {
  return _ag1171State == Ag1171State::Awake ||
         _ag1171State == Ag1171State::PollingHook;
}

const char* HardwareControl::ag1171PowerStateName() const {
  switch (_ag1171State) {
    case Ag1171State::Awake:       return "AWAKE";
    case Ag1171State::PoweredDown: return "POWERED DOWN";
    case Ag1171State::Waking:      return "WAKING";
    case Ag1171State::PollingHook: return "POLLING SHK";
    default:                       return "UNKNOWN";
  }
}

void HardwareControl::requestAg1171Wake() {
  const uint32_t now = millis();
  if (_ag1171State == Ag1171State::PoweredDown) {
    startWake(now);
  } else if (_ag1171State == Ag1171State::PollingHook) {
    _ag1171State = Ag1171State::Awake;
    _stateChangedAt = now;
  }
}

void HardwareControl::triggerHardwareReset() {
  LogSerial.println("HARD RESET: triggering A4 battery power cycle");
  digitalWrite(PIN_HARD_RESET_TRIGGER, HIGH);
  delay(HARD_RESET_TRIGGER_PULSE_MS);
  digitalWrite(PIN_HARD_RESET_TRIGGER, LOW);
}

void HardwareControl::releasePowerDownPin() {
  // INPUT is the ESP32 equivalent of the open circuit required by the AG1171.
  pinMode(PIN_AG1171_PD, INPUT);
}

void HardwareControl::assertPowerDownPin() {
  digitalWrite(PIN_AG1171_PD, LOW);
  pinMode(PIN_AG1171_PD, OUTPUT);
}

void HardwareControl::startWake(uint32_t now) {
  releasePowerDownPin();
  _ag1171State = Ag1171State::Waking;
  _stateChangedAt = now;
}

void HardwareControl::enterPoweredDown(uint32_t now) {
  assertPowerDownPin();
  _ag1171State = Ag1171State::PoweredDown;
  _stateChangedAt = now;
}
