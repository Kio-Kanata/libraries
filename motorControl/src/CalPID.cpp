#include "CalPID.h"

CalPID::CalPID(pidParam_t pidParam_, double dt_) {
    setParam(pidParam_);
    setDt(dt_);
}

CalPID::CalPID() {}

void CalPID::setParam(pidParam_t pidParam_) {
    _pidParam = pidParam_;
}

void CalPID::setDt(double dt_) {
    _dt = dt_;
}

double CalPID::calc(double deviation_) {
    _deviation.prev = _deviation.curr;
    _deviation.curr = deviation_;
    if (abs(_val) < _pidParam.max) _integral += (_deviation.curr + _deviation.prev) * _dt / 2.0;
    _differential = (_deviation.curr - _deviation.prev) / _dt;
    _val = _pidParam.kp * _deviation.curr + _pidParam.ki * _integral + _pidParam.kd * _differential;
    if (_val > _pidParam.max) _val = _pidParam.max;
    if (_val < -1 * _pidParam.max) _val = -1 * _pidParam.max;
    return _val;
}

void CalPID::resetIntegral() {
    _integral = 0.0;
}