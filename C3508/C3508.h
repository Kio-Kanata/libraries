#ifndef _C3508_H_
#define _C3508_H_

#include "C620.h"
#include "CalPID.h"

class C3508 {
public:
  enum DIRECTION { FWD = 1, REV = -1 };
  enum MODE { UNUSED, SLEEP, CURRENT, SPEED, ANGLE };
  typedef struct { double angle, speed, current; } target_t;
  typedef struct {
    uint8_t id;
    MODE mode;
    double gearRatio;
    DIRECTION direction;
    pidParam_t anglePid, speedPid, currentPid; 
    target_t target;
  } motor_t;
  typedef struct { uint16_t angle, speed, current; } pidInterval_t;

  bool needToSend = false;
  
  C3508();
  void setMotor(motor_t *motor_);
  void setPidInterval(pidInterval_t pidInterval_);
  void init();
  void refresh(uint32_t micros_, struct can_frame *sendMsg_, struct can_frame *readMsg_ = nullptr);
  double readAngle();
  double readSpeed();
  double readCurrent();
//private:
  C620 _driver;
  struct { CalPID angle, speed, current; } _pid = {};
  motor_t *_motor = nullptr;
  pidInterval_t _pidInterval;
  struct { double angle, speed, current; } _dt = {};
  struct { uint32_t angle, speed, current; } _microsPrev = {};
  uint16_t _refreshCount = 0;
  struct { double raw, scaled; } _angle, _speed = {};
  double _current;
  uint16_t _rpmLimit = 600;
  uint8_t _gearRatio = 19;
  double _angleRawPrev;
  int64_t _rotate;
  double _calcDt(uint32_t micros_, uint32_t microsPrev_);
  double _calAngle(double angleRawCurr_);
  void _read();
};

#endif