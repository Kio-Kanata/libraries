#ifndef _C620_H_
#define _C620_H_

#include <Arduino.h>
#include <mcp2515.h>

class C620 {
private:
    typedef struct { uint8_t upper, lower; } byte_t;
    uint8_t _id;
    double _angle;
    int16_t _speed;
    double _current;
    int8_t _temperature;
    byte_t _divide(int16_t val);
    int16_t _connect(byte_t byte_);
public:
    C620();
    void setID(uint8_t id_);
    void setCurrent(struct can_frame *sendMsg_, double current_);
    void update(struct can_frame *readMsg_);
    uint8_t readId();
    double readAngle();
    int16_t readSpeed();
    double readCurrent();
    int8_t readTemperature();
} ;

#endif