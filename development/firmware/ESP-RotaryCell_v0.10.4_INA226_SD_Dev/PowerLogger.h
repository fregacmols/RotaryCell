#pragma once

#include <Arduino.h>
#include <FS.h>

class PowerLogger {
public:
  bool begin();
  void update();
  bool isReady() const;
  const char* filename() const;

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
};
