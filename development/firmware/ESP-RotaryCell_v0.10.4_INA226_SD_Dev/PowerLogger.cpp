#include "PowerLogger.h"

#include "Config.h"
#include "LogSerial.h"
#include <SD.h>
#include <SPI.h>
#include <Wire.h>

namespace {
constexpr uint8_t INA226_REG_CONFIG       = 0x00;
constexpr uint8_t INA226_REG_SHUNT_VOLTAGE = 0x01;
constexpr uint8_t INA226_REG_BUS_VOLTAGE = 0x02;
constexpr uint8_t INA226_REG_POWER       = 0x03;
constexpr uint8_t INA226_REG_CURRENT     = 0x04;
constexpr uint8_t INA226_REG_CALIBRATION = 0x05;

// 16-sample averaging, 1.1 ms bus and shunt conversions, continuous mode.
constexpr uint16_t INA226_CONFIG = 0x0527;

// 0.1 mA/current-register bit with the fitted 0.002 ohm (R002) shunt.
// CAL = 0.00512 / (0.0001 A * 0.002 ohm) = 25600.
constexpr float INA226_CURRENT_LSB_A = 0.0001f;
constexpr float INA226_POWER_LSB_W = 25.0f * INA226_CURRENT_LSB_A;
constexpr uint16_t INA226_CALIBRATION = 25600;
}

bool PowerLogger::begin() {
  Wire.begin(PIN_QWIIC_SDA, PIN_QWIIC_SCL);
  Wire.setClock(400000);

  _inaReady = beginIna226();
  _sdReady = beginSdCard();

  if (!_inaReady) {
    LogSerial.println("POWER LOG: INA226 not found at I2C address 0x40");
  }
  if (!_sdReady) {
    LogSerial.println("POWER LOG: microSD card unavailable");
  }

  if (!isReady()) {
    if (_file) {
      _file.close();
    }
    LogSerial.println("POWER LOG: disabled; telephone firmware will continue normally");
    return false;
  }

  const char* header = "millis,bus_voltage_V,current_A,power_W,shunt_voltage_mV\r\n";
  if (!appendLine(header, strlen(header)) || !flushBuffer()) {
    LogSerial.println("POWER LOG: could not write CSV header");
    _file.close();
    _sdReady = false;
    return false;
  }

  _lastSampleAt = millis();
  _lastFlushAt = _lastSampleAt;
  LogSerial.print("POWER LOG: recording to ");
  LogSerial.println(_filename);
  return true;
}

void PowerLogger::update() {
  if (!isReady()) {
    return;
  }

  const uint32_t now = millis();
  if (now - _lastSampleAt < POWER_LOG_SAMPLE_INTERVAL_MS) {
    return;
  }
  _lastSampleAt = now;

  uint16_t rawBus = 0;
  uint16_t rawCurrent = 0;
  uint16_t rawPower = 0;
  uint16_t rawShunt = 0;
  if (!readRegister(INA226_REG_BUS_VOLTAGE, rawBus) ||
      !readRegister(INA226_REG_CURRENT, rawCurrent) ||
      !readRegister(INA226_REG_POWER, rawPower) ||
      !readRegister(INA226_REG_SHUNT_VOLTAGE, rawShunt)) {
    LogSerial.println("POWER LOG: INA226 read failed; logging stopped");
    flushBuffer();
    _inaReady = false;
    return;
  }

  const float busVoltageV = static_cast<float>(rawBus) * 0.00125f;
  const float currentA = static_cast<float>(static_cast<int16_t>(rawCurrent)) *
                         INA226_CURRENT_LSB_A;
  const float powerW = static_cast<float>(rawPower) * INA226_POWER_LSB_W;
  const float shuntVoltageMv = static_cast<float>(static_cast<int16_t>(rawShunt)) *
                               0.0025f;

  char line[112];
  const int length = snprintf(line, sizeof(line), "%lu,%.5f,%.5f,%.5f,%.5f\r\n",
                              static_cast<unsigned long>(now), busVoltageV,
                              currentA, powerW, shuntVoltageMv);
  if (length <= 0 || static_cast<size_t>(length) >= sizeof(line) ||
      !appendLine(line, static_cast<size_t>(length))) {
    LogSerial.println("POWER LOG: microSD write failed; logging stopped");
    _file.close();
    _sdReady = false;
    return;
  }

  if (now - _lastFlushAt >= POWER_LOG_FLUSH_INTERVAL_MS) {
    if (!flushBuffer()) {
      LogSerial.println("POWER LOG: microSD flush failed; logging stopped");
      _file.close();
      _sdReady = false;
      return;
    }
    _lastFlushAt = now;
  }
}

bool PowerLogger::isReady() const {
  return _inaReady && _sdReady && _file;
}

const char* PowerLogger::filename() const {
  return _filename;
}

bool PowerLogger::beginIna226() {
  Wire.beginTransmission(INA226_I2C_ADDRESS);
  if (Wire.endTransmission() != 0) {
    return false;
  }

  return writeRegister(INA226_REG_CONFIG, INA226_CONFIG) &&
         writeRegister(INA226_REG_CALIBRATION, INA226_CALIBRATION);
}

bool PowerLogger::beginSdCard() {
  SPI.begin(PIN_SD_SCK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);
  if (!SD.begin(PIN_SD_CS, SPI, SD_SPI_FREQUENCY_HZ)) {
    return false;
  }

  for (uint16_t number = 1; number <= 999; ++number) {
    snprintf(_filename, sizeof(_filename), "/POWER%03u.CSV", number);
    if (!SD.exists(_filename)) {
      _file = SD.open(_filename, FILE_WRITE);
      return static_cast<bool>(_file);
    }
  }

  LogSerial.println("POWER LOG: POWER001.CSV through POWER999.CSV already exist");
  return false;
}

bool PowerLogger::writeRegister(uint8_t reg, uint16_t value) {
  Wire.beginTransmission(INA226_I2C_ADDRESS);
  Wire.write(reg);
  Wire.write(static_cast<uint8_t>(value >> 8));
  Wire.write(static_cast<uint8_t>(value & 0xff));
  return Wire.endTransmission() == 0;
}

bool PowerLogger::readRegister(uint8_t reg, uint16_t& value) {
  Wire.beginTransmission(INA226_I2C_ADDRESS);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) {
    return false;
  }

  if (Wire.requestFrom(INA226_I2C_ADDRESS, static_cast<uint8_t>(2)) != 2) {
    return false;
  }

  value = static_cast<uint16_t>(Wire.read()) << 8;
  value |= static_cast<uint16_t>(Wire.read());
  return true;
}

bool PowerLogger::appendLine(const char* line, size_t length) {
  if (length > sizeof(_buffer)) {
    return false;
  }

  if (_bufferUsed + length > sizeof(_buffer) && !flushBuffer()) {
    return false;
  }

  memcpy(_buffer + _bufferUsed, line, length);
  _bufferUsed += length;
  return true;
}

bool PowerLogger::flushBuffer() {
  if (_bufferUsed == 0) {
    return true;
  }

  const size_t written = _file.write(
    reinterpret_cast<const uint8_t*>(_buffer), _bufferUsed);
  if (written != _bufferUsed) {
    return false;
  }

  _file.flush();
  _bufferUsed = 0;
  return true;
}
