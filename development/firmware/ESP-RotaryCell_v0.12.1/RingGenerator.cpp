#include "RingGenerator.h"
#include "Config.h"

#include "LogSerial.h"
void RingGenerator::begin() {
  pinMode(PIN_FR, OUTPUT);
  pinMode(PIN_RM, OUTPUT);
  endRingOutput();
}

void RingGenerator::startTest() {
  stop();
  _mode = Mode::Test;
  _modeStartedAt = millis();
  beginRingOutput();
  LogSerial.println("RING: one-second test started");
}

void RingGenerator::startCadence() {
  stop();
  _mode = Mode::SingleCadenceRing;
  _modeStartedAt = millis();
  beginRingOutput();
  LogSerial.println("RING: single cadence started (2 seconds on, 4 seconds off)");
}

void RingGenerator::startIncomingCadence() {
  stop();
  _mode = Mode::IncomingRing;
  _modeStartedAt = millis();
  beginRingOutput();
  LogSerial.println("RING: incoming-call cadence started");
}

void RingGenerator::stop() {
  endRingOutput();
  _mode = Mode::Idle;
}

void RingGenerator::update() {
  const uint32_t now = millis();

  switch (_mode) {
    case Mode::Idle:
      return;

    case Mode::Test:
      updateRingWave(now);
      if (now - _modeStartedAt >= RING_TEST_DURATION_MS) {
        stop();
        LogSerial.println("RING: test complete");
      }
      return;

    case Mode::SingleCadenceRing:
      updateRingWave(now);
      if (now - _modeStartedAt >= CADENCE_RING_ON_MS) {
        endRingOutput();
        _mode = Mode::SingleCadencePause;
        _modeStartedAt = now;
        LogSerial.println("RING: cadence pause");
      }
      return;

    case Mode::SingleCadencePause:
      if (now - _modeStartedAt >= CADENCE_RING_OFF_MS) {
        stop();
        LogSerial.println("RING: cadence complete");
      }
      return;

    case Mode::IncomingRing:
      updateRingWave(now);
      if (now - _modeStartedAt >= CADENCE_RING_ON_MS) {
        endRingOutput();
        _mode = Mode::IncomingPause;
        _modeStartedAt = now;
      }
      return;

    case Mode::IncomingPause:
      if (now - _modeStartedAt >= CADENCE_RING_OFF_MS) {
        _mode = Mode::IncomingRing;
        _modeStartedAt = now;
        beginRingOutput();
      }
      return;
  }
}

void RingGenerator::beginRingOutput() {
  _phase = false;
  _lastToggleAt = millis();
  digitalWrite(PIN_FR, LOW);
  digitalWrite(PIN_RM, HIGH);
}

void RingGenerator::endRingOutput() {
  digitalWrite(PIN_RM, LOW);
  digitalWrite(PIN_FR, LOW);
  _phase = false;
}

void RingGenerator::updateRingWave(uint32_t now) {
  if (now - _lastToggleAt >= RING_HALF_PERIOD_MS) {
    _lastToggleAt = now;
    _phase = !_phase;
    digitalWrite(PIN_FR, _phase ? HIGH : LOW);
  }
}

bool RingGenerator::isActive() const {
  return _mode != Mode::Idle;
}

bool RingGenerator::isIncomingCadence() const {
  return _mode == Mode::IncomingRing || _mode == Mode::IncomingPause;
}

const char* RingGenerator::modeName() const {
  switch (_mode) {
    case Mode::Idle:                return "IDLE";
    case Mode::Test:                return "TEST";
    case Mode::SingleCadenceRing:   return "SINGLE CADENCE RING";
    case Mode::SingleCadencePause:  return "SINGLE CADENCE PAUSE";
    case Mode::IncomingRing:        return "INCOMING CALL RING";
    case Mode::IncomingPause:       return "INCOMING CALL PAUSE";
  }
  return "UNKNOWN";
}
