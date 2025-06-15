#include "C620.h"

C620::C620() {}

C620::byte_t C620::_divide(int16_t val) {
  byte_t byte;
  byte.upper = (uint8_t)((val >> 8) & 0xFF);
  byte.lower = (uint8_t)(val & 0xFF);
  return byte;
}

int16_t C620::_connect(byte_t byte_) {
  return (int16_t)(byte_.upper << 8 | byte_.lower);
}

void C620::setID(uint8_t id_) {
  _id = id_;
}

void C620::setCurrent(struct can_frame *sendMsg_, double current_) {
  int16_t encodedCurrent = (int16_t)(16384 * current_ / 20);
  byte_t byte = _divide(encodedCurrent);
  sendMsg_->data[((_id - 1) % 4) * 2] = byte.upper;
  sendMsg_->data[((_id - 1) % 4) * 2 + 1] = byte.lower;
}

void C620::update(struct can_frame *readMsg_) {
  byte_t byte[3];
  for (uint8_t i = 0; i < 3; i++) {
    byte[i].upper = readMsg_->data[2 * i];
    byte[i].lower = readMsg_->data[2 * i + 1];
  }
  _angle = (360.0 / 8192.0) * (double)_connect(byte[0]);
  _speed = (double)_connect(byte[1]);
  _current = (20.0 / 16384.0) * (double)_connect(byte[2]);
  _temperature = (double)readMsg_->data[6];
}

uint8_t C620::readId() {
  return _id;
}

double C620::readAngle() {
  return _angle;
}

int16_t C620::readSpeed() {
  return _speed;
}

double C620::readCurrent() {
  return _current;
}

int8_t C620::readTemperature() {
  return _temperature;
}