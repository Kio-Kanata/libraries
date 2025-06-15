#ifndef __I2C_READWRITE_H__
#define __I2C_READWRITE_H__

#include <Wire.h>

class I2C_ReadWrite {
public:
  I2C_ReadWrite(uint8_t addr, TwoWire *wire);
  uint8_t getAddr();
  bool isAvailable();
  uint8_t write(uint8_t val);
  uint8_t write(uint8_t reg, uint8_t val);
  uint8_t read(uint8_t reg, uint8_t length);
  uint8_t data[256] = {};
private:
  TwoWire *_wire;
  uint8_t _addr;
};

#endif