#ifndef _MOTOR_CONTROL_H_
#define _MOTOR_CONTROL_H_

#ifndef MAX_MOTOR_COUNT
#define MAX_MOTOR_COUNT 8
#endif

#define READMESSAGE_BASE_ID 0x200
#define SENDMESSAGE_1_TO_4_ID 0x200
#define SENDMESSAGE_5_TO_8_ID 0x1FF

#include "M3508.h"

class motorControl {
public:
    bool needToSend[2] = { false, false };
    void setMotor(M3508::motor_t motor_[], uint8_t motorCount_);
    void setPidInterval(M3508::pidInterval_t pidInterval_);
    void init();
    uint8_t refresh(uint32_t micros_, struct can_frame sendMsg_[], struct can_frame *readMsg_ = nullptr);
    double readAngle(uint8_t motorNum_);
    double readSpeed(uint8_t motorNum_);
    double readCurrent(uint8_t motorNum_);
    double readOutputCurrent(uint8_t motorNum_);
//private:
    struct { uint8_t all, ch1, ch2; } _motorCount = {};
    M3508 _m3508[MAX_MOTOR_COUNT] = {};
    uint8_t _idQueue[MAX_MOTOR_COUNT] = {};
    uint8_t _requestCount[2] = {};
};

#endif