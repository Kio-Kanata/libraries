#ifndef __BLUETOOTH_PS_CONTROLLER_H__
#define __BLUETOOTH_PS_CONTROLLER_H__

#include <Arduino.h>

typedef void (*PSButtonHandler_t)();

enum class BUTTON {
    UP, RIGHT, DOWN, LEFT,
    TRIANGLE, CIRCLE, X, SQUARE,
    L1, L3,
    R1, R3,
    L2, R2,
    LS, RS
};

enum class DIRECTION {
    X, Y,
    UP, RIGHT, DOWN, LEFT
};

enum class STATUS {
    PRESSED, HELD, RELEASED
};

class BluetoothPSController {
public:
    BluetoothPSController();
    void onRawReport(const uint8_t* rpt_, uint16_t n_, uint32_t millis_);
    void setDeadzone(BUTTON button_, double val_);
    void setDeadzone(BUTTON button_, DIRECTION axis_, double val_);
    void setButtonThreshold(BUTTON button_, double val_);
    void setButtonThreshold(BUTTON button_, DIRECTION direction_, double val_);
    void attachButtonHandler(BUTTON button_, STATUS status_, PSButtonHandler_t buttonHandler_, uint16_t interval_ = 0);
    void attachButtonHandler(BUTTON button_, DIRECTION direction_, STATUS status_, PSButtonHandler_t buttonHandler_, uint16_t interval_ = 0);
    double getValue(BUTTON button_);
    double getValue(BUTTON button_, DIRECTION axis_);
    bool isPressed(BUTTON button_);
    bool isPressed(BUTTON button_, DIRECTION direction_);
private:
    uint8_t _rpt[11];
    double _buttonThreshold[10];
    double _value[6] = {};
    bool _isPressed[22] = {};
    bool _wasPressed[22] = {};
    double _deadzone[6] = {};
    bool _isAttachedButtonHandler[22][3] = {};
    PSButtonHandler_t _buttonHandler[22][3];
    uint16_t _buttonHandlerInterval[22] = {};
    uint32_t _millisPrev = 0;
    uint16_t _duration[22] = {};
     
    bool _getBit(uint8_t val_, uint8_t pos_);
    double _normalize(uint8_t val_, bool isSigned);
    double _abs(double val_);
    void _convertData();
    void _applyDeadzone();
    void _callButtonHandler(uint32_t dt_);
};

#endif
