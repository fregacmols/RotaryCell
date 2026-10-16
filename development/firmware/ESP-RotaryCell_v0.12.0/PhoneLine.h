#pragma once
#include <Arduino.h>

class PhoneLine {
public:
  void begin();
  void update();

  bool isOffHook() const;
  bool rawOffHook() const;
  uint8_t pendingPulseCount() const;

  bool takeCompletedDigit(uint8_t& digit);

  void setVerbosePulses(bool enabled);
  bool verbosePulses() const;

private:
  bool readOffHook() const;
  void processStableTransition(bool newOffHook, uint32_t now);
  void finishDigit();

  bool _rawOffHook = false;
  bool _lastRawOffHook = false;
  bool _stableOffHook = false;
  bool _reportedOffHook = false;
  bool _awaitingPulseReturn = false;
  bool _verbosePulses = false;
  bool _digitReady = false;

  uint8_t _pulseCount = 0;
  uint8_t _completedDigit = 0;

  uint32_t _rawChangedAt = 0;
  uint32_t _breakStartedAt = 0;
  uint32_t _lastPulseAt = 0;
};
