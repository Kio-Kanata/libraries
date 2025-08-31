#ifndef _M3508_H_
#define _M3508_H_

#include "C620.h"
#include "CalPID.h"

class M3508 {
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
  
  M3508();
  void setMotor(motor_t *motor_);
  void setPidInterval(pidInterval_t pidInterval_);
  void init();
  void refresh(uint32_t micros_, struct can_frame *sendMsg_, struct can_frame *readMsg_);
  double readAngle();
  double readSpeed();
  double readCurrent();
  double readOutputCurrent();
private:
  typedef struct { uint32_t curr, prev, step; } micros_t;
  C620 _driver;
  struct { CalPID angle, speed, current, read; } _pid = {};
  motor_t *_motor = nullptr;
  pidInterval_t _pidInterval;
  micros_t _ms = {};
  struct { struct { uint64_t  angle, speed, current, read; } curr, prev; } _micros = {};
  struct { double angle, speed, current, read; } _dt = {};
  uint64_t _refreshCount = 0;
  struct { double raw, scaled; } _angle, _speed = {};
  double _current;
  double _outputCurrent;
  double _angleRawPrev;
  uint64_t _calcMicros(uint32_t micros_, micros_t *ms_);
  double _calcDt(uint64_t curr_, uint64_t prev_);
  double _calAngleError(double angleRawCurr_);
  void _read();
};

#endif