#ifndef _MAG_ENCODER_H_
#define _MAG_ENCODER_H_

#include <Arduino.h>

class magEncoder {
public:
    magEncoder(uint8_t _Apin, uint8_t _Bpin);
    void setGearRatio(double gearRatio);
    void calc();
    void read();
    double rotate();
    double rps();
    uint8_t Apin;
    uint8_t Bpin;
private:
    int32_t _count;
    double _gearRatio = 1;
    double _rotate;
    double _pre_rotate = 0;
    double _rps;
    uint32_t _cur_micros;
    uint32_t _pre_micros = 0;
};

#endif