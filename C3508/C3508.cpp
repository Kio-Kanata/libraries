#include "C3508.h"

C3508::C3508() {}

void C3508::setMotor(motor_t *motor_) {
    _motor = motor_;
}

void C3508::setPidInterval(pidInterval_t pidInterval_) {
    _pidInterval = pidInterval_;
}

void C3508::init() {
    _driver.setID(_motor->id);
    _pid.angle.setParam(_motor->anglePid);
    _pid.speed.setParam(_motor->speedPid);
    _pid.current.setParam(_motor->currentPid);
}

void C3508::refresh(uint32_t micros_, struct can_frame *sendMsg_, struct can_frame *readMsg_) {
    _refreshCount++;
    needToSend = false;
    if (_refreshCount % _pidInterval.angle == 0) {
        _dt.angle = _calcDt(micros_, _microsPrev.angle);
        _pid.angle.setDt(_dt.angle);
    }
    if (_refreshCount % _pidInterval.speed == 0) {
        _dt.speed = _calcDt(micros_, _microsPrev.speed);
        _pid.speed.setDt(_dt.speed);
    }
    if (_refreshCount % _pidInterval.current == 0) {
        _dt.current = _calcDt(micros_, _microsPrev.current);
        _pid.current.setDt(_dt.current);
    }
    if (_refreshCount % _pidInterval.speed == 0)
    if (_refreshCount % _pidInterval.current == 0)
    if ((readMsg_ !=nullptr) && (_motor->mode != UNUSED)) {
        _driver.update(readMsg_);
        _read();
    }
    if (_refreshCount % _pidInterval.angle == 0) {
        if (_motor->mode == ANGLE) {
            _motor->target.speed = _pid.angle.calc(_motor->target.angle - _angle.scaled);
        }
        _refreshCount = 0;
    }
    if (_refreshCount % _pidInterval.speed == 0) {
        if ((_motor->mode == SPEED) || (_motor->mode == ANGLE)) {
            _motor->target.current = _pid.speed.calc(_motor->target.speed - _speed.scaled);
        }
    }
    if (_refreshCount % _pidInterval.current == 0) {
        if ((_motor->mode == CURRENT) || (_motor->mode == SPEED) || (_motor->mode == ANGLE)) {
            double outputCurrent = _pid.current.calc(_motor->target.current - _current);
            _driver.setCurrent(sendMsg_, outputCurrent);
            needToSend = true;
        }
    }

}

double C3508::readAngle() {
    return _angle.scaled;
}

double C3508::readSpeed() {
    return _speed.scaled;
}

double C3508::readCurrent() {
    return _current;
}

double C3508::_calcDt(uint32_t micros_, uint32_t microsPrev_) {
    if (micros_ > microsPrev_) return (double)(micros_ - _microsPrev) * 0.000001;
    else return (double)((int64_t)(micros_ - _microsPrev) + 4294967296) * 0.000001;
}

double C3508::_calAngle(double angleRawCurr_) {
    int16_t errorLimit = _rpmLimit * _gearRatio / 60 * 360 * _dt.current;
    int16_t error = (int16_t)(angleRawCurr_ - _angleRawPrev);
    _angleRawPrev = angleRawCurr_;
    if (error < -1 * errorLimit) _rotate++;
    if (error > errorLimit) _rotate--;
    //return 360.0 * _rotate + angleRawCurr_;
    return error;
}

void C3508::_read() {
    _angle.raw = _calAngle(_driver.readAngle());
    _speed.raw = (double)_driver.readSpeed();
    _angle.scaled = _motor->direction * _angle.raw / (double)_motor->gearRatio;
    _speed.scaled = _motor->direction * _speed.raw / (double)_motor->gearRatio;
    _current = _driver.readCurrent();
}