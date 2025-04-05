#ifndef _MOTOR_DRIBER_H_
#define _MOTOR_DRIVER_H_

#include <Arduino.h>

typedef uint8_t pin_t;

class motorDriver {
public:
    motorDriver(pin_t D1, pin_t D2, pin_t PWM, bool direction = true);
    void off();
    void stop();
    void outFwd(uint8_t pwm);
    void outRev(uint8_t pwm);
    void out(int16_t pwm);
private:
    pin_t _D1;
    pin_t _D2;
    pin_t _PWM;
    bool _direction;
};

#endif