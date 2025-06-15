#ifndef _MOTOR_CONTROL_H_
#define _MOTOR_CONTROL_H_

#ifndef MAX_MOTOR_COUNT
#define MAX_MOTOR_COUNT 8
#endif

#define READMESSAGE_BASE_ID 0x200;
#define SENDMESSAGE_1_TO_4_ID 0x200;
#define SENDMESSAGE_1_TO_4_ID 0x1FF;

#include "C3508.h"

class motorControl {
public:
    bool needToSend = false;

    void setMotor(C3508::motor_t motor_[], uint8_t motorCount_);
    void setDt(double dt_);
    void setPidInterval(C3508::pidInterval_t pidInterval_);
    void init();
    void refresh(struct can_flame sendMsg_[], struct can_flame *readMsg_ = nullptr);
private:
    C3508::motor_t *_motor[MAX_MOTOR_COUNT] = {};
    uint8_t _motorCount = 0;
    C3508 _c3508[MAX_MOTOR_COUNT] = {};
    uint8_t _idQueue[MAX_MOTOR_COUNT] = {};
};

#endif