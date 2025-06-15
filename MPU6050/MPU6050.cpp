#include <MPU6050.h>

MPU6050::MPU6050(TwoWire *wire, bool addrSelect) {
  _wire = wire;
  if (addrSelect) _addr = 0x69;
}

uint8_t MPU6050::getAddress() {
  return _addr;
}

void MPU6050::setResolution(bool sensor, uint8_t resolution) {
  if (resolution < 4) {
    _resolution[sensor] = resolution;
    if (sensor == ACCEL) _sentVal[2] = (uint8_t)(resolution << 3);
    else _sentVal[1] = (uint8_t)(resolution << 3);
  }
}

uint8_t MPU6050::getResolution(bool sensor) {
  return _resolution[sensor];
}

void MPU6050::setError(bool sensor, uint8_t axis, double error) {
  _error[sensor * 3 + axis] = error;
}

double MPU6050::getError(bool sensor, uint8_t axis) {
  return _error[sensor * 3 + axis];
}

void MPU6050::begin() {
  for (uint8_t i = 0; i < 4; i++) _write(_sentReg[i], _sentVal[i]);
}

void MPU6050::read(uint32_t time) {
  _dt = (double)(time - _time_previous) / 1000000.0;
  _getData();
  _convert();
  _integrate();
  _time_previous = time;
}

double MPU6050::get(bool sensor, uint8_t axis, bool isIntegrated ) {
  if (isIntegrated == false) return _converted[sensor * 3 + axis];
  else if (sensor == GYRO) return _integrated[axis];
  else return -1.0;
}

void MPU6050::_write(uint8_t reg, uint8_t val) {
  _wire->beginTransmission(_addr);
  _wire->write(reg);
  _wire->write(val);
  _wire->endTransmission();
}

void MPU6050::_getData() {
  _wire->beginTransmission(_addr);
  _wire->write(0x3B);
  _wire->endTransmission();
  _wire->requestFrom(_addr, 14, true);
  _raw[0] = (int16_t)(_wire->read() << 8 | _wire->read());
  _raw[1] = (int16_t)(_wire->read() << 8 | _wire->read());
  _raw[2] = (int16_t)(_wire->read() << 8 | _wire->read());
  _wire->read();
  _wire->read();
  _raw[3] = (int16_t)(_wire->read() << 8 | _wire->read());
  _raw[4] = (int16_t)(_wire->read() << 8 | _wire->read());
  _raw[5] = (int16_t)(_wire->read() << 8 | _wire->read());
}

void MPU6050::_convert() {
  for (uint8_t i = 0; i < 3; i++) _converted[i] = (double)_raw[i] * (double)(2 * (_resolution[0] + 1)) / 32768.0 - _error[i];
  for (uint8_t i = 0; i < 3; i++) _converted[i + 3] = (double)_raw[i + 3] * (double)(250 * (_resolution[0] + 1)) / 32768.0 - _error[i + 3];
}

void MPU6050::_integrate() {
  for (uint8_t i = 0; i < 3; i++) _integrated[i] += _converted[i + 3] * _dt;
}