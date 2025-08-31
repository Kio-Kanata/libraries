#include "M3508.h"

M3508::M3508() {}

void M3508::setMotor(motor_t *motor_) {
    _motor = motor_;
}

void M3508::setPidInterval(pidInterval_t pidInterval_) {
    _pidInterval = pidInterval_;
}

void M3508::init() {
    _driver.setID(_motor->id);
    _pid.angle.setParam(_motor->anglePid);
    _pid.speed.setParam(_motor->speedPid);
    _pid.current.setParam(_motor->currentPid);
}

void M3508::refresh(uint32_t micros_, struct can_frame *sendMsg_, struct can_frame *readMsg_) {
    _refreshCount++;
    needToSend = false;
    uint64_t ms = _calcMicros(micros_, &_ms);
    if (_refreshCount % _pidInterval.angle == 0) {
        _micros.prev.angle = _micros.curr.angle;
        _micros.curr.angle = ms;
        _dt.angle = _calcDt(_micros.curr.angle, _micros.prev.angle);
        if (_dt.angle < 1.0) _pid.angle.setDt(_dt.angle);
        else _pid.angle.resetIntegral();
    }
    if (_refreshCount % _pidInterval.speed == 0) {
        _micros.prev.speed = _micros.curr.speed;
        _micros.curr.speed = ms;
        _dt.speed = _calcDt(_micros.curr.speed, _micros.prev.speed);
        if (_dt.speed < 1.0) _pid.speed.setDt(_dt.speed);
        else _pid.speed.resetIntegral();
    }
    if (_refreshCount % _pidInterval.current == 0) {
        _micros.prev.current = _micros.curr.current;
        _micros.curr.current = ms;
        _dt.current = _calcDt(_micros.curr.current, _micros.prev.current);
        if (_dt.current < 1.0) _pid.current.setDt(_dt.current);
        else _pid.current.resetIntegral();
    }
    if (_motor->mode != UNUSED) {
        _micros.prev.read = _micros.curr.read;
        _micros.curr.read = ms;
        _dt.read = _calcDt(_micros.curr.read, _micros.prev.read);
        if (_dt.read < 1.0) _driver.update(readMsg_);
        _read();
    }
    if (_refreshCount % _pidInterval.angle == 0) {
        if (_motor->mode == ANGLE) {
            _motor->target.speed = _pid.angle.calc(_motor->target.angle - _angle.scaled);
        }
    }
    if (_refreshCount % _pidInterval.speed == 0) {
        if ((_motor->mode == SPEED) || (_motor->mode == ANGLE)) {
            _motor->target.current = _pid.speed.calc(_motor->target.speed - _speed.scaled);
        }
    }
    if (_refreshCount % _pidInterval.current == 0) {
        if ((_motor->mode == CURRENT) || (_motor->mode == SPEED) || (_motor->mode == ANGLE)) {
            _outputCurrent = _pid.current.calc(_motor->target.current - _current);
            _driver.setCurrent(sendMsg_, _outputCurrent);
            needToSend = true;
        }
    }

}

double M3508::readAngle() {
    return _angle.scaled;
}

double M3508::readSpeed() {
    return _speed.scaled;
}

double M3508::readCurrent() {
    return _current;
}

double M3508::readOutputCurrent() {
    return _outputCurrent;
}

uint64_t M3508::_calcMicros(uint32_t micros_, micros_t *ms_) {
    ms_->prev = ms_->curr;
    ms_->curr = micros_;
    if (ms_->curr < ms_->prev) ms_->step++;
    uint64_t ms = (((uint64_t)ms_->step << 32) | (uint64_t)ms_->curr);
    return ms;
}

double M3508::_calcDt(uint64_t curr_, uint64_t prev_) {
    return (double)(curr_ - prev_) / 1000000.0;
}

double M3508::_calAngleError(double angleRawCurr_) {
    double error = angleRawCurr_ - _angleRawPrev;
    _angleRawPrev = angleRawCurr_;
    if ((_speed.raw < 0.0) && (error > +90.0)) error -= 360.0;
    if ((_speed.raw > 0.0) && (error < -90.0)) error += 360.0;
    return error;
}

void M3508::_read() {
    _angle.raw += _calAngleError(_driver.readAngle());
    _speed.raw = (double)_driver.readSpeed();
    _angle.scaled = _motor->direction * _angle.raw / (double)_motor->gearRatio;
    _speed.scaled = _motor->direction * _speed.raw / (double)_motor->gearRatio;
    _current = _driver.readCurrent();
}