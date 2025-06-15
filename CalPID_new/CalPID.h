#ifndef CALPID_H
#define CALPID_H

#include <cmath>

typedef struct { double kp, ki, kd, max; } pidParam_t;

class CalPID
{
private:
    pidParam_t _pidParam;
    double _dt;
    struct {
        double curr = 0;
        double prev = 0;
    } _deviation;
    double _integral = 0;
    double _differential = 0;
    double _val = 0;

public:
    CalPID(pidParam_t pidParam_, double dt_);
    CalPID();
    void setParam(pidParam_t pidParam_);
    void setDt(double dt_);
    double calc(double deviation_);
};

#endif