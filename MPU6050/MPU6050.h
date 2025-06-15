#ifndef __MPU6050_H__
#define __MPU6050_H__

#include <Wire.h>

#define ACCEL 0
#define GYRO 1

#define ACCEL_2 0
#define ACCEL_4 1
#define ACCEL_8 2
#define ACCEL_16 3

#define GYRO_250 0
#define GYRO_500 1
#define GYRO_1000 2
#define GYRO_2000 3

#define X 0
#define Y 1
#define Z 2

#define DEG 1

class MPU6050 {
public:
  MPU6050(TwoWire *wire, bool addrSelect = false);
  uint8_t getAddress();
  void setResolution(bool sensor, uint8_t resolution);
  uint8_t getResolution(bool sensor);
  void setError(bool sensor, uint8_t axis, double error);
  double getError(bool sensor, uint8_t axis);
  void begin();
  void read(uint32_t time);
  double get(bool sensor, uint8_t axis, bool isIntegrated = false);
//private:
  void _write(uint8_t reg, uint8_t val);
  void _getData();
  void _convert();
  void _integrate();
  TwoWire *_wire;
  uint8_t _addr = 0x68;
  uint8_t _resolution[2] = {};
  double _error[6] = {};
  uint32_t _time_previous = 0;
  double _dt = 0;
  uint8_t _sentReg[4] = {0x6B, 0x1A, 0x1B, 0x1C};
  uint8_t _sentVal[4] = {0x00, 0x01, 0x00, 0x00};
  int16_t _raw[6];
  double _converted[6];
  double _integrated[3];
};

#endif