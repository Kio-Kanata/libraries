#include "BluetoothPSController.h"

BluetoothPSController::BluetoothPSController() {
    for (uint8_t i = 0; i < 10; i++) _buttonThreshold[i] = 0.8;
}

void BluetoothPSController::onRawReport(const uint8_t* rpt_, uint16_t n_, uint32_t millis_) {
    if (n_ != 11) return;
    for (uint8_t i = 0; i < 11; i++) _rpt[i] = rpt_[i];
    _convertData();
    _applyDeadzone();
    _callButtonHandler(millis_ - _millisPrev);
    _millisPrev = millis_;
}

void BluetoothPSController::setDeadzone(BUTTON button_, double val_) {
    if (val_ > 0.0) {
        if (button_ == BUTTON::L2) _deadzone[0] = val_;
        if (button_ == BUTTON::R2) _deadzone[1] = val_;
    }
}

void BluetoothPSController::setDeadzone(BUTTON button_, DIRECTION axis_, double val_) {
    if (val_ > 0.0) {
        if (button_ == BUTTON::LS && axis_ == DIRECTION::X) _deadzone[2] = val_;
        if (button_ == BUTTON::LS && axis_ == DIRECTION::Y) _deadzone[3] = val_;
        if (button_ == BUTTON::RS && axis_ == DIRECTION::X) _deadzone[4] = val_;
        if (button_ == BUTTON::RS && axis_ == DIRECTION::Y) _deadzone[5] = val_;
    }
}

void BluetoothPSController::setButtonThreshold(BUTTON button_, double val_) {
    if ((button_ == BUTTON::L2) && (val_ > 0.0)) _buttonThreshold[0] = val_;
    if ((button_ == BUTTON::R2) && (val_ > 0.0)) _buttonThreshold[1] = val_;
}

void BluetoothPSController::setButtonThreshold(BUTTON button_, DIRECTION direction_, double val_) {
    uint8_t direction = static_cast<uint8_t>(direction_);
    if ((button_ == BUTTON::LS) && (direction > 1) && (val_ > 0.0)) _buttonThreshold[direction] = val_;
    if ((button_ == BUTTON::RS) && (direction > 1) && (val_ > 0.0)) _buttonThreshold[direction + 4] = val_;
}

void BluetoothPSController::attachButtonHandler(BUTTON button_, STATUS status_, PSButtonHandler_t buttonHandler_, uint16_t interval_) {
    uint8_t button = static_cast<uint8_t>(button_);
    uint8_t status = static_cast<uint8_t>(status_);
    if (button < 14) {
        _isAttachedButtonHandler[button][status] = true;
        _buttonHandler[button][status] = buttonHandler_;
        if (status == 1) _buttonHandlerInterval[button] = interval_;
    }
}

void BluetoothPSController::attachButtonHandler(BUTTON button_, DIRECTION direction_, STATUS status_, PSButtonHandler_t buttonHandler_, uint16_t interval_) {
    uint8_t direction = static_cast<uint8_t>(direction_);
    uint8_t status = static_cast<uint8_t>(status_);
    if (button_ == BUTTON::LS && (direction > 1)) {
        _isAttachedButtonHandler[direction + 12][status] = true;
        _buttonHandler[direction + 12][status] = buttonHandler_;
        if (status == 1) _buttonHandlerInterval[14] = interval_;
    }
    if (button_ == BUTTON::RS && (direction > 1)) {
        _isAttachedButtonHandler[direction + 16][status] = true;
        _buttonHandler[direction + 16][status] = buttonHandler_;
        if (status == 1) _buttonHandlerInterval[15] = interval_;
    }
}

double BluetoothPSController::getValue(BUTTON button_) {
    if (button_ == BUTTON::L2) return _value[0];
    if (button_ == BUTTON::R2) return _value[1];
    return 0.0;
}

double BluetoothPSController::getValue(BUTTON button_, DIRECTION axis_) {
    if (button_ == BUTTON::LS && axis_ == DIRECTION::X) return _value[2];
    if (button_ == BUTTON::LS && axis_ == DIRECTION::Y) return _value[3];
    if (button_ == BUTTON::RS && axis_ == DIRECTION::X) return _value[4];
    if (button_ == BUTTON::RS && axis_ == DIRECTION::Y) return _value[5];
    return 0.0;
}

bool BluetoothPSController::isPressed(BUTTON button_) {
    uint8_t button = static_cast<uint8_t>(button_);
    if (button < 14) return _isPressed[button];
    return false;
}

bool BluetoothPSController::isPressed(BUTTON button_, DIRECTION direction_) {
    uint8_t direction = static_cast<uint8_t>(direction_);
    if ((button_ == BUTTON::LS) && (direction > 1)) return _isPressed[direction + 12];
    if ((button_ == BUTTON::RS) && (direction > 1)) return _isPressed[direction + 16];
    return false;
}

bool BluetoothPSController::_getBit(uint8_t val_, uint8_t pos_) {
  return (val_ >> pos_) & 0x01;
}

double BluetoothPSController::_normalize(uint8_t val_, bool isSigned) {
    double normalized = ((double)val_ / 256.0);
    if (isSigned) return normalized * 2.0 - 1.0;
    return normalized;
}

double BluetoothPSController::_abs(double val_) {
    if (val_ >= 0.0) return val_;
    return -val_;
}

void BluetoothPSController::_convertData() {
    _value[0] = _normalize(_rpt[9], false);
    _value[1] = _normalize(_rpt[10], false);
    _value[2] = _normalize(_rpt[2], true);
    _value[3] = _normalize(_rpt[3], true);
    _value[4] = _normalize(_rpt[4], true);
    _value[5] = _normalize(_rpt[5], true);

    for (uint8_t i = 0; i < 22; i++) _wasPressed[i] = _isPressed[i];

    uint8_t arrow = _rpt[6] & 0x0F;
    _isPressed[0] = (arrow == 0 || arrow == 1 || arrow == 7);
    _isPressed[1] = (arrow == 1 || arrow == 2 || arrow == 3);
    _isPressed[2] = (arrow == 3 || arrow == 4 || arrow == 5);
    _isPressed[3] = (arrow == 5 || arrow == 6 || arrow == 7);

    _isPressed[4] = _getBit(_rpt[6], 7);
    _isPressed[5] = _getBit(_rpt[6], 6);
    _isPressed[6] = _getBit(_rpt[6], 5);
    _isPressed[7] = _getBit(_rpt[6], 4);

    _isPressed[8] = _getBit(_rpt[7], 0);
    _isPressed[9] = _getBit(_rpt[7], 6);
    _isPressed[10] = _getBit(_rpt[7], 1);
    _isPressed[11] = _getBit(_rpt[7], 7);

    _isPressed[12] = _value[0] > _buttonThreshold[0];
    _isPressed[13] = _value[1] > _buttonThreshold[1];
    _isPressed[14] = _value[3] < -_buttonThreshold[2];
    _isPressed[15] = _value[2] > _buttonThreshold[3];
    _isPressed[16] = _value[3] > _buttonThreshold[4];
    _isPressed[17] = _value[2] < -_buttonThreshold[5];
    _isPressed[18] = _value[5] < -_buttonThreshold[6];
    _isPressed[19] = _value[4] > _buttonThreshold[7];
    _isPressed[20] = _value[5] > _buttonThreshold[8];
    _isPressed[21] = _value[4] < -_buttonThreshold[9];
}

void BluetoothPSController::_applyDeadzone() {
    for (uint8_t i = 0; i < 6; i++) {
        if (_abs(_value[i]) < _deadzone[i]) _value[i] = 0.0;
    }
}

void BluetoothPSController::_callButtonHandler(uint32_t dt_) {
    for (uint8_t i = 0; i < 22; i++) {
        if (_isAttachedButtonHandler[i][0] && _isPressed[i] && !_wasPressed[i]) _buttonHandler[i][0]();
        if (_isAttachedButtonHandler[i][1]) {
            if (_isPressed[i]) {
                _duration[i] += dt_;
                if (_duration[i] >= _buttonHandlerInterval[i]) {
                    _buttonHandler[i][1]();
                    _duration[i] -= _buttonHandlerInterval[i];
                }
            }
            else _duration[i] = 0;
        }
        if (_isAttachedButtonHandler[i][0] && !_isPressed[i] && _wasPressed[i]) _buttonHandler[i][2]();
    }   
}
