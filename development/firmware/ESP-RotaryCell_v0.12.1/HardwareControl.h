#pragma once

#include <Arduino.h>

class HardwareControl {
public:
  void begin();
  void update(bool mustStayAwake, bool offHook);

  void setPowerSavingEnabled(bool enabled);
  void setPollInterval(uint16_t ms) { _pollInterval = ms; }
  bool powerSavingEnabled() const;
  bool hookSignalValid() const;
  const char* ag1171PowerStateName() const;

  void requestAg1171Wake();
  void triggerHardwareReset();

private:
  enum class Ag1171State : uint8_t {
    Awake,
    PoweredDown,
    Waking,
    PollingHook
  };

  void releasePowerDownPin();
  void assertPowerDownPin();
  void startWake(uint32_t now);
  void enterPoweredDown(uint32_t now);

  Ag1171State _ag1171State = Ag1171State::Awake;
  bool _powerSavingEnabled = true;
  uint16_t _pollInterval = 500;
  uint32_t _stateChangedAt = 0;
};
