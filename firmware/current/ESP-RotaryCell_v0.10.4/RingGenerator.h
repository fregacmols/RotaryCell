#pragma once
#include <Arduino.h>

class RingGenerator {
public:
  enum class Mode : uint8_t {
    Idle,
    Test,
    SingleCadenceRing,
    SingleCadencePause,
    IncomingRing,
    IncomingPause
  };

  void begin();
  void update();

  void startTest();
  void startCadence();
  void startIncomingCadence();
  void stop();

  bool isActive() const;
  bool isIncomingCadence() const;
  const char* modeName() const;

private:
  void beginRingOutput();
  void endRingOutput();
  void updateRingWave(uint32_t now);

  Mode _mode = Mode::Idle;
  bool _phase = false;
  uint32_t _modeStartedAt = 0;
  uint32_t _lastToggleAt = 0;
};
