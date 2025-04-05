#include "motorDriver.h"

motorDriver::motorDriver(pin_t D1, pin_t D2, pin_t PWM, bool direction) {
    _D1 = D1;
    _D2 = D2;
    _PWM = PWM;
    _direction = direction;
    pinMode(_D1, OUTPUT);
    pinMode(_D2, OUTPUT);
    pinMode(_PWM, OUTPUT);
}

void motorDriver::off() {
    digitalWrite(_D1, LOW);
    digitalWrite(_D2, LOW);
    analogWrite(_PWM, 0);
}

void motorDriver::stop() {
    digitalWrite(_D1, HIGH);
    digitalWrite(_D2, HIGH);
    analogWrite(_PWM, 255);
}

void motorDriver::outFwd(uint8_t pwm) {
    digitalWrite(_D1, LOW);
    digitalWrite(_D2, HIGH);
    analogWrite(_PWM, pwm);
}

void motorDriver::outRev(uint8_t pwm) {
    digitalWrite(_D1, HIGH);
    digitalWrite(_D2, LOW);
    analogWrite(_PWM, pwm);
}

void motorDriver::out(int16_t pwm) {
    if (pwm > 255) pwm = 255;
    else if (pwm < -255) pwm = -255;

    if (pwm == 0) stop();
    else if ((pwm > 0) && (_direction == true)) outFwd(pwm);
    else if ((pwm > 0) && (_direction == false)) outRev(pwm);
    else if ((pwm < 0) && (_direction == true)) outRev(abs(pwm));
    else if ((pwm < 0) && (_direction == false)) outFwd(abs(pwm));
}