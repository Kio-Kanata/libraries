#include "I2C_ReadWrite.h"

I2C_ReadWrite::I2C_ReadWrite(uint8_t addr, TwoWire *wire) {
  _addr = addr;
  _wire = wire;
}

uint8_t I2C_ReadWrite::getAddr() {
  return _addr;
}

bool I2C_ReadWrite::isAvailable() {
  _wire->beginTransmission(_addr);
  if (_wire->endTransmission() == 0) return true;
  else return false;
}

uint8_t I2C_ReadWrite::write(uint8_t val) {
  _wire->beginTransmission(_addr);
  _wire->write(val);
  return _wire->endTransmission();
}

uint8_t I2C_ReadWrite::write(uint8_t reg, uint8_t val) {
  _wire->beginTransmission(_addr);
  _wire->write(reg);
  _wire->write(val);
  return _wire->endTransmission();
}

uint8_t I2C_ReadWrite::read(uint8_t reg, uint8_t length) {
  while (_wire->available()) _wire->read();
  if (write(reg) != 0) return 0;
  _wire->requestFrom(_addr, length);
  uint8_t i = 0;
  while (_wire->available()) data[i++] = _wire->read();
  return i;
}