#pragma once

#include <Arduino.h>
#include <FS.h>

class PowerLogger {
public:
  bool sdReady() const { return _sdReady; }
  bool flushForDownload() { return !_file || flushBuffer(); }
  bool begin();
  void update();
  bool isReady() const;
  bool hasSample() const;
  const char* filename() const;
  float latestBusVoltageV() const;
  float latestCurrentA() const;
  float latestPowerW() const;

private:
  bool beginIna226();
  bool beginSdCard();
  bool writeRegister(uint8_t reg, uint16_t value);
  bool readRegister(uint8_t reg, uint16_t& value);
  bool appendLine(const char* line, size_t length);
  bool flushBuffer();

  File _file;
  char _filename[16] = "";
  char _buffer[4096];
  size_t _bufferUsed = 0;
  uint32_t _lastSampleAt = 0;
  uint32_t _lastFlushAt = 0;
  bool _inaReady = false;
  bool _sdReady = false;
  bool _hasSample = false;
  float _latestBusVoltageV = 0.0f;
  float _latestCurrentA = 0.0f;
  float _latestPowerW = 0.0f;
};
