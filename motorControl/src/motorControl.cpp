#include "motorControl.h"

void motorControl::setMotor(M3508::motor_t motor_[], uint8_t motorCount_) {
    _motorCount.all = motorCount_;
    for (uint8_t i = 0; i < _motorCount.all; i++) _idQueue[i] = 255;
    for (uint8_t i = 0; i < _motorCount.all; i++) {
        _m3508[i].setMotor(&motor_[i]);
        if ((1 <= motor_[i].id) && (motor_[i].id <= 8)) _idQueue[motor_[i].id - 1] = i;
        if ((1 <= motor_[i].id) && (motor_[i].id <= 4)) _motorCount.ch1++;
        if ((5 <= motor_[i].id) && (motor_[i].id <= 8)) _motorCount.ch2++;
    }
}

void motorControl::setPidInterval(M3508::pidInterval_t pidInterval_) {
    for (uint8_t i = 0; i < _motorCount.all; i++) _m3508[i].setPidInterval(pidInterval_);
}

void motorControl::init() {
    for (uint8_t i = 0; i < _motorCount.all; i++) _m3508[i].init();
}

uint8_t motorControl::refresh(uint32_t micros_, struct can_frame sendMsg_[], struct can_frame *readMsg_) {
    uint16_t driverId = readMsg_->can_id - READMESSAGE_BASE_ID;
    if ((1 <= driverId) && (driverId <= 8)) {
        _m3508[_idQueue[driverId - 1]].refresh(micros_, &sendMsg_[(driverId - 1) / 4], readMsg_);
        if (_m3508[_idQueue[driverId - 1]].needToSend) {
            if ((1 <= driverId) && (driverId <= 4)) _requestCount[0]++;
            if ((5 <= driverId) && (driverId <= 8)) _requestCount[1]++;
        }
        if ((_motorCount.ch1 > 0) && (_requestCount[0] >= _motorCount.ch1)) {
            needToSend[0] = true;
            _requestCount[0] = 0;
        }
        else needToSend[0] = false;
        if ((_motorCount.ch2 > 0) && (_requestCount[1] >= _motorCount.ch2)) {
            needToSend[1] = true;
            _requestCount[1] = 0;
        }
        else needToSend[1] = false;
    }
    return driverId;
}

double motorControl::readAngle(uint8_t motorNum_) {
    if ((0 <= motorNum_) && (motorNum_ < _motorCount.all)) return _m3508[motorNum_].readAngle();
    else return 0.0;
}

double motorControl::readSpeed(uint8_t motorNum_) {
    if ((0 <= motorNum_) && (motorNum_ < _motorCount.all)) return _m3508[motorNum_].readSpeed();
    else return 0.0;
}

double motorControl::readCurrent(uint8_t motorNum_) {
    if ((0 <= motorNum_) && (motorNum_ < _motorCount.all)) return _m3508[motorNum_].readCurrent();
    else return 0.0;
}

double motorControl::readOutputCurrent(uint8_t motorNum_) {
    if ((0 <= motorNum_) && (motorNum_ < _motorCount.all)) return _m3508[motorNum_].readOutputCurrent();
    else return 0.0;
}