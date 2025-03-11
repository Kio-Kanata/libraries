#include "magEncoder.h"

magEncoder::magEncoder(uint8_t _Apin, uint8_t _Bpin) {
    Apin = _Apin;
    Bpin = _Bpin;
}

void magEncoder::setGearRatio(double gearRatio) {
    _gearRatio = gearRatio;
}

void magEncoder::calc() {
    if(digitalRead(Apin) ^ digitalRead(Bpin)) _count++;
    else _count--;
}

void magEncoder::read() {
    _cur_micros = micros();
    _rotate = (double)(_count) / _gearRatio;
    _rps = (_rotate - _pre_rotate) / ((double)(_cur_micros - _pre_micros) * 0.000001);
    _pre_micros = _cur_micros;
    _pre_rotate = _rotate;
}

double magEncoder::rotate() {
    return _rotate;
}

double magEncoder::rps() {
    return _rps;
}