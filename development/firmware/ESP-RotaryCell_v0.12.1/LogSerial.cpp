#include "LogSerial.h"
LogSerialClass LogSerial;

void LogSerialClass::begin(unsigned long baud) {
  Serial.begin(baud);
}

int LogSerialClass::available() { return Serial.available(); }
int LogSerialClass::read() { return Serial.read(); }
int LogSerialClass::peek() { return Serial.peek(); }
void LogSerialClass::flush() { Serial.flush(); }
LogSerialClass::operator bool() const { return static_cast<bool>(Serial); }

void LogSerialClass::setTimestampProvider(TimestampProvider provider) {
  _timestampProvider = provider;
}

void LogSerialClass::capture(uint8_t c) {
  _buffer[_nextSequence % BUFFER_SIZE] = static_cast<char>(c);
  ++_nextSequence;
}

void LogSerialClass::emitPrefixIfNeeded(uint8_t nextChar) {
  if (!_lineStart || nextChar == '\r' || nextChar == '\n') {
    return;
  }

  String prefix;
  if (_timestampProvider) {
    prefix = _timestampProvider();
  }
  if (prefix.length() == 0) {
    return;
  }
  prefix += ' ';
  Serial.write(reinterpret_cast<const uint8_t*>(prefix.c_str()), prefix.length());
  for (size_t i = 0; i < prefix.length(); ++i) {
    capture(static_cast<uint8_t>(prefix[i]));
  }
  _lineStart = false;
}

size_t LogSerialClass::write(uint8_t c) {
  emitPrefixIfNeeded(c);
  const size_t result = Serial.write(c);
  capture(c);
  if (c == '\n') {
    _lineStart = true;
  } else if (c != '\r') {
    _lineStart = false;
  }
  return result;
}

size_t LogSerialClass::write(const uint8_t* buffer, size_t size) {
  size_t written = 0;
  for (size_t i = 0; i < size; ++i) {
    written += write(buffer[i]);
  }
  return written;
}

uint64_t LogSerialClass::oldestSequence() const {
  return _nextSequence > BUFFER_SIZE ? _nextSequence - BUFFER_SIZE : 0;
}

uint64_t LogSerialClass::newestSequence() const {
  return _nextSequence;
}

String LogSerialClass::readFrom(uint64_t sequence, size_t maxBytes, uint64_t& actualStart, uint64_t& actualEnd) const {
  const uint64_t oldest = oldestSequence();
  const uint64_t newest = newestSequence();
  actualStart = sequence < oldest ? oldest : sequence;
  if (actualStart > newest) {
    actualStart = newest;
  }
  uint64_t count = newest - actualStart;
  if (count > maxBytes) {
    count = maxBytes;
  }
  actualEnd = actualStart + count;

  String result;
  result.reserve(static_cast<size_t>(count));
  for (uint64_t seq = actualStart; seq < actualEnd; ++seq) {
    result += _buffer[seq % BUFFER_SIZE];
  }
  return result;
}

void LogSerialClass::clearHistory() {
  _nextSequence = 0;
  _lineStart = true;
}
