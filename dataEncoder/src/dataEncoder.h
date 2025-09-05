#ifndef _DATA_ENCODER_H_
#define _DATA_ENCODER_H_

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

class dataEncoder {
public:
  typedef struct {
    struct {
      uint8_t length, data[MAX_DATA_NUM * 2 + 4];
    } toInit;
    struct {
      uint8_t length, data[MAX_BYTE];
    } toSend;
  } dataPacket_t;

  dataEncoder(uint8_t id_);
  template <typename T>
  void append(uint8_t ord_, T *dataPtr_, uint8_t size_ = 0); //送信するデータを追加する
  void init(); //送信するデータ構成を確定し，初期化コードを生成する．
  void encode(); //送信するデータをエンコードし，送信コードを生成する．
  dataPacket_t getPacket();
private:
  enum TYPE {
    NONE = 00,
    BOOL = 10,
    UINT8_T = 20,
    UINT16_T = 21,
    UINT32_T = 22,
    UINT64_T = 23,
    INT8_T = 30,
    INT16_T = 31,
    INT32_T = 32,
    INT64_T = 33,
    FLOAT = 40,
    DOUBLE = 41
  };
  uint8_t _id; //エンコーダのid
  TYPE _type[MAX_DATA_NUM]; //データの型
  void *_ptr[MAX_DATA_NUM]; //データのポインタ
  bool _isEditable;
  struct {
    uint16_t bitSum;
    struct {
      uint8_t preEncoded, encoded;
    } data[MAX_DATA_NUM];
  } _length;
  bool _binary[MAX_BIT];
  dataPacket_t _dataPacket = {};
  template <typename T>
  TYPE identifyType();
  template <typename T>
  uint8_t decideSize(TYPE type_, uint8_t size_);
  template <typename T>
  uint32_t getRawData(uint8_t ord_);
  uint32_t getData(uint8_t ord_);
  void generateBinary();
};

#endif