#ifndef _DATA_DECODER_H_
#define _DATA_DECODER_H_

#include <Arduino.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#ifndef MAX_DATA_NUM
#define MAX_DATA_NUM 10
#endif
#ifndef MAX_BIT
#define MAX_BIT MAX_DATA_NUM * 32
#endif
#ifndef MAX_BYTE
#define MAX_BYTE MAX_BIT / 7 + 5
#endif

#ifndef MAX_PORT_NUM
#define MAX_PORT_NUM 10
#endif

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 255
#endif

class dataDecoder {
public:
  dataDecoder();
  void appendToBuffer(uint8_t data);
  bool decode();
  template <typename T>
  T decodedData(uint8_t id_, uint8_t ord_);
private:
  enum TYPE {
    NONE = 0,
    BOOL = 1,
    UINT = 2,
    INT = 3,
    FLOAT = 4
  };
  TYPE _type[MAX_PORT_NUM][MAX_DATA_NUM];
  struct {
    uint16_t bitSum;
    uint8_t encoded[MAX_DATA_NUM];
  } _length[MAX_PORT_NUM];
  struct {
    uint8_t buffer[BUFFER_SIZE];
    uint8_t toRead[MAX_BYTE];
    bool binary[MAX_BIT];
    uint32_t dividedBinary[MAX_PORT_NUM][MAX_DATA_NUM];
  } _data;
  uint8_t _bufferIndex;
  uint8_t _extractData();
  void _shiftLeftArray(uint8_t step_);
  void _regId(uint8_t id_);
  void _read(uint8_t id_);
  void _divide(uint8_t id_);
  template <typename T>
  T _cast(uint8_t id_, uint8_t ord_);
};

#endif