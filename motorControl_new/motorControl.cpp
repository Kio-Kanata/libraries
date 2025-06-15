#include "motorControl.h"

void motorControl::setMotor(C3508::motor_t motor_[], uint8_t motorCount_) {
    _motorCount = motorCount_;
    for (uint8_t i = 0; i < _motorCount; i++) {
    _motor[i] = &motor_[i];
    _idQueue[_motor[i]->id] = i;
  }
}

void motorControl::setDt(double dt_) {
    for (uint8_t i = 0; i < _motorCount; i++) _c3508.setDt(dt_);
}

void motorControl::setPidInterval(C3508::pidInterval_t pidInterval_) {
    for (uint8_t i = 0; i < _motorCount; i++) _c3508.setPidInterval(pidInterval_);
}

void motorControl::init() {
    for (uint8_t i = 0; i < _motorCount; i++) _c3508.init();
}

void motorControl::refresh(struct can_flame sendMsg_[], struct can_flame *readMsg_ = nullptr) {
    uint16_t driverId = 0;
    if (readMsg_ != nullptr) driverId = readMsg_->can_id - READMESSAGE_BASE_ID;
    for (uint8_t i = 0; i < _motorCount; i++) {
        if ()
    }
}