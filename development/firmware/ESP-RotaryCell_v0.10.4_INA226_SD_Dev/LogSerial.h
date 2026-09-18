#pragma once
#include <Arduino.h>

class LogSerialClass : public Stream {
public:
  using TimestampProvider = String (*)();

  void begin(unsigned long baud);
  int available() override;
  int read() override;
  int peek() override;
  void flush() override;
  size_t write(uint8_t c) override;
  size_t write(const uint8_t* buffer, size_t size) override;
  using Print::write;
  operator bool() const;

  void setTimestampProvider(TimestampProvider provider);
  uint64_t oldestSequence() const;
  uint64_t newestSequence() const;
  String readFrom(uint64_t sequence, size_t maxBytes, uint64_t& actualStart, uint64_t& actualEnd) const;
  void clearHistory();

private:
  static constexpr size_t BUFFER_SIZE = 32768;
  char _buffer[BUFFER_SIZE]{};
  uint64_t _nextSequence = 0;
  bool _lineStart = true;
  TimestampProvider _timestampProvider = nullptr;

  void capture(uint8_t c);
  void emitPrefixIfNeeded(uint8_t nextChar);
};

extern LogSerialClass LogSerial;
