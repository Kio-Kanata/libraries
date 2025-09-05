#include "dataEncoder.h"

dataEncoder::dataEncoder(uint8_t id_){
  _id = id_;
  _isEditable = true;
  for (uint8_t i = 0; i < MAX_DATA_NUM; i++) {
    _type[i] = NONE;
  }
}

template <typename T>
void dataEncoder::append(uint8_t ord_, T *dataPtr_, uint8_t size_) {
  if (_isEditable) {
    _type[ord_] = identifyType<T>();
    _ptr[ord_] = (void*)dataPtr_;
    _length.data[ord_].preEncoded = sizeof(T) * 8;
    _length.data[ord_].encoded = decideSize<T>(_type[ord_], size_);
  }
}

template void dataEncoder::append<bool>(uint8_t ord_, bool *dataPtr_, uint8_t size_);
template void dataEncoder::append<uint8_t>(uint8_t ord_, uint8_t *dataPtr_, uint8_t size_);
template void dataEncoder::append<uint16_t>(uint8_t ord_, uint16_t *dataPtr_, uint8_t size_);
template void dataEncoder::append<uint32_t>(uint8_t ord_, uint32_t *dataPtr_, uint8_t size_);
template void dataEncoder::append<uint64_t>(uint8_t ord_, uint64_t *dataPtr_, uint8_t size_);
template void dataEncoder::append<int8_t>(uint8_t ord_, int8_t *dataPtr_, uint8_t size_);
template void dataEncoder::append<int16_t>(uint8_t ord_, int16_t *dataPtr_, uint8_t size_);
template void dataEncoder::append<int32_t>(uint8_t ord_, int32_t *dataPtr_, uint8_t size_);
template void dataEncoder::append<int64_t>(uint8_t ord_, int64_t *dataPtr_, uint8_t size_);
template void dataEncoder::append<float>(uint8_t ord_, float *dataPtr_, uint8_t size_);
template void dataEncoder::append<double>(uint8_t ord_, double *dataPtr_, uint8_t size_);

void dataEncoder::init() {
  _isEditable = false;
  _length.bitSum = 0;
  for (uint8_t i = 0; i < MAX_DATA_NUM; i++) {
    _length.bitSum += _length.data[i].encoded;
  }
  uint8_t num = 0;
  _dataPacket.toInit.data[num] = 0b11000000 | _id;
  num++;
  for (uint8_t i = 0; i < MAX_DATA_NUM; i++) {
    _dataPacket.toInit.data[num] = (_type[i] / 10);
    num++;
    _dataPacket.toInit.data[num] = _length.data[i].encoded;
    num++;
  }
  uint16_t checkSum = 0;
  for (uint8_t i = 1; i < num; i++) {
    checkSum += _dataPacket.toInit.data[i];
  }
  _dataPacket.toInit.data[num] = (uint8_t)((checkSum >> 8) & 0xff);
  num++;
  _dataPacket.toInit.data[num] = (uint8_t)(checkSum & 0xff);
  num++;
  _dataPacket.toInit.data[num] = 0b10000000 | (num + 1);
  _dataPacket.toInit.length = num + 1;
}

void dataEncoder::encode() {
  for (uint8_t i = 0; i < MAX_BYTE; i++) {
    _dataPacket.toSend.data[i] = 0;
  }
  generateBinary();
  uint8_t num = 0;
  _dataPacket.toSend.data[num] = 0b10000000 | _id;
  num++;
  for (uint16_t i = 0; i < _length.bitSum; i++) {
    _dataPacket.toSend.data[num] |= _binary[i] << (6 - (i % 7));
    if ((i % 7) == 6) num++;
  }
  if ((_length.bitSum % 7) != 0) num++;
  uint32_t checkSum = 0;
  for (uint8_t i = 1; i < num; i++) {
    checkSum += _dataPacket.toSend.data[i];
  }
  _dataPacket.toSend.data[num] = (uint8_t)((checkSum >> 14) & 0x7f);
  num++;
  _dataPacket.toSend.data[num] = (uint8_t)((checkSum >> 7) & 0x7f);
  num++;
  _dataPacket.toSend.data[num] = (uint8_t)((checkSum) & 0x7f);
  num++;
  _dataPacket.toSend.data[num] = 0b10000000 | (num + 1);
  _dataPacket.toSend.length = num + 1;
}

dataEncoder::dataPacket_t dataEncoder::getPacket() {
  return _dataPacket;
}

template <typename T>
dataEncoder::TYPE dataEncoder::identifyType() {return NONE;}
template <>
dataEncoder::TYPE dataEncoder::identifyType<bool>() {return BOOL;}
template <>
dataEncoder::TYPE dataEncoder::identifyType<uint8_t>() {return UINT8_T;}
template <>
dataEncoder::TYPE dataEncoder::identifyType<uint16_t>() {return UINT16_T;}
template <>
dataEncoder::TYPE dataEncoder::identifyType<uint32_t>() {return UINT32_T;}
template <>
dataEncoder::TYPE dataEncoder::identifyType<uint64_t>() {return UINT64_T;}
template <>
dataEncoder::TYPE dataEncoder::identifyType<int8_t>() {return INT8_T;}
template <>
dataEncoder::TYPE dataEncoder::identifyType<int16_t>() {return INT16_T;}
template <>
dataEncoder::TYPE dataEncoder::identifyType<int32_t>() {return INT32_T;}
template <>
dataEncoder::TYPE dataEncoder::identifyType<int64_t>() {return INT64_T;}
template <>
dataEncoder::TYPE dataEncoder::identifyType<float>() {return FLOAT;}
template <>
dataEncoder::TYPE dataEncoder::identifyType<double>() {return DOUBLE;}

template <typename T>
uint8_t dataEncoder::decideSize(dataEncoder::TYPE type_, uint8_t size_) {
  uint8_t size;
  if (type_ == BOOL) size = 1;
  else if (type_ == UINT64_T) size = 32;
  else if (type_ == INT64_T) size = 32;
  else if (type_ == FLOAT) size = 32;
  else if (type_ == DOUBLE) size = 32;
  else if (size_ == 0) size = sizeof(T) * 8;
  else size = size_;
  return size;
}

template uint8_t dataEncoder::decideSize<bool>(dataEncoder::TYPE type_, uint8_t size_);
template uint8_t dataEncoder::decideSize<uint8_t>(dataEncoder::TYPE type_, uint8_t size_);
template uint8_t dataEncoder::decideSize<uint16_t>(dataEncoder::TYPE type_, uint8_t size_);
template uint8_t dataEncoder::decideSize<uint32_t>(dataEncoder::TYPE type_, uint8_t size_);
template uint8_t dataEncoder::decideSize<uint64_t>(dataEncoder::TYPE type_, uint8_t size_);
template uint8_t dataEncoder::decideSize<int8_t>(dataEncoder::TYPE type_, uint8_t size_);
template uint8_t dataEncoder::decideSize<int16_t>(dataEncoder::TYPE type_, uint8_t size_);
template uint8_t dataEncoder::decideSize<int32_t>(dataEncoder::TYPE type_, uint8_t size_);
template uint8_t dataEncoder::decideSize<int64_t>(dataEncoder::TYPE type_, uint8_t size_);
template uint8_t dataEncoder::decideSize<float>(dataEncoder::TYPE type_, uint8_t size_);
template uint8_t dataEncoder::decideSize<double>(dataEncoder::TYPE type_, uint8_t size_);

template <typename T>
uint32_t dataEncoder::getRawData(uint8_t ord_) {
  T devidedBinary = 0;
  if (_type[ord_] == BOOL) devidedBinary = (T)*static_cast<bool*>(_ptr[ord_]);
  if (_type[ord_] == UINT8_T) devidedBinary = (T)*static_cast<uint8_t*>(_ptr[ord_]);
  if (_type[ord_] == UINT16_T) devidedBinary = (T)*static_cast<uint16_t*>(_ptr[ord_]);
  if (_type[ord_] == UINT32_T) devidedBinary = (T)*static_cast<uint32_t*>(_ptr[ord_]);
  if (_type[ord_] == UINT64_T) devidedBinary = (T)*static_cast<uint64_t*>(_ptr[ord_]);
  if (_type[ord_] == INT8_T) devidedBinary = (T)*static_cast<int8_t*>(_ptr[ord_]);
  if (_type[ord_] == INT16_T) devidedBinary = (T)*static_cast<int16_t*>(_ptr[ord_]);
  if (_type[ord_] == INT32_T) devidedBinary = (T)*static_cast<int32_t*>(_ptr[ord_]);
  if (_type[ord_] == INT64_T) devidedBinary = (T)*static_cast<int64_t*>(_ptr[ord_]);
  if (_type[ord_] == FLOAT) devidedBinary = (T)*static_cast<float*>(_ptr[ord_]);
  if (_type[ord_] == DOUBLE) devidedBinary = (T)*static_cast<double*>(_ptr[ord_]);
  union {T typed; uint32_t untyped;} data;
  data.typed = devidedBinary;
  return data.untyped;
}

template uint32_t dataEncoder::getRawData<uint32_t>(uint8_t ord_);
template uint32_t dataEncoder::getRawData<int32_t>(uint8_t ord_);
template uint32_t dataEncoder::getRawData<float>(uint8_t ord_);

uint32_t dataEncoder::getData(uint8_t ord_) {
  uint32_t devidedBinary = 0;
  if ((_type[ord_] / 10) == 1) devidedBinary = getRawData<uint32_t>(ord_);
  if ((_type[ord_] / 10) == 2) devidedBinary = getRawData<uint32_t>(ord_);
  if ((_type[ord_] / 10) == 3) {
    devidedBinary = getRawData<int32_t>(ord_);
    bool fill = bitRead(devidedBinary, 31);
    for (uint8_t i = _length.data[ord_].encoded; i < 32; i++) {
      devidedBinary |= (fill << i);
    }
  }
  if ((_type[ord_] / 10) == 4) devidedBinary = getRawData<float>(ord_);
  return devidedBinary;
}

void dataEncoder::generateBinary() {
  uint16_t bitNum = 0;
  for (uint8_t i = 0; i < MAX_DATA_NUM; i++) {
    if (_type[i] != NONE) {
      for (uint8_t bit = 0; bit < _length.data[i].encoded; bit++) {
        _binary[bitNum] = bitRead(getData(i), _length.data[i].encoded - bit - 1);
        bitNum++;
      }
    }
  }
}