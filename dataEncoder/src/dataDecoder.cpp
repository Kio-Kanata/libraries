#include "dataDecoder.h"

dataDecoder::dataDecoder() {
    for (uint8_t i = 0; i < MAX_PORT_NUM; i++) {
        for (uint8_t j = 0; j < MAX_DATA_NUM; j++) {
            _type[i][j] = NONE;
            _length[i].encoded[j] = 0;
        }
        _length[i].bitSum = 0;
    }
    _bufferIndex = 0;
}

void dataDecoder::appendToBuffer(uint8_t data) {
    _data.buffer[_bufferIndex] = data;
    _bufferIndex++;
}

bool dataDecoder::decode() {
    bool error = _extractData();
    if (!error) {
        uint8_t id = _data.toRead[0] & 0x0F;
        if (((_data.toRead[0] >> 6) & 0x03) == 0b11) {
            _regId(id);
        }
        if (((_data.toRead[0] >> 6) & 0x03) == 0b10) {
            _read(id);
            _divide(id);
        }
    }
    return error;
}

template <typename T>
T dataDecoder::decodedData(uint8_t id_, uint8_t ord_) {
    T result;
    if (_type[id_][ord_] == BOOL) result = T(_cast<bool>(id_, ord_));
    else if (_type[id_][ord_] == UINT) result = T(_cast<uint32_t>(id_, ord_));
    else if (_type[id_][ord_] == INT) result = T(_cast<int32_t>(id_, ord_));
    else if (_type[id_][ord_] == FLOAT) result = T(_cast<float>(id_, ord_));
    return result;
}

template bool dataDecoder::decodedData(uint8_t id_, uint8_t ord_);
template uint8_t dataDecoder::decodedData(uint8_t id_, uint8_t ord_);
template uint16_t dataDecoder::decodedData(uint8_t id_, uint8_t ord_);
template uint32_t dataDecoder::decodedData(uint8_t id_, uint8_t ord_);
template uint64_t dataDecoder::decodedData(uint8_t id_, uint8_t ord_);
template int8_t dataDecoder::decodedData(uint8_t id_, uint8_t ord_);
template int16_t dataDecoder::decodedData(uint8_t id_, uint8_t ord_);
template int32_t dataDecoder::decodedData(uint8_t id_, uint8_t ord_);
template int64_t dataDecoder::decodedData(uint8_t id_, uint8_t ord_);
template float dataDecoder::decodedData(uint8_t id_, uint8_t ord_);
template double dataDecoder::decodedData(uint8_t id_, uint8_t ord_);

uint8_t dataDecoder::_extractData() {
    uint8_t error = 0;
    for (uint8_t i = 0; i < MAX_BYTE; i++) _data.toRead[i] = 0;

    uint8_t idx = 0;
    while ((_data.buffer[idx] & 0b10000000) == 0) {
        if (idx == (BUFFER_SIZE - 1)) {
            error = 1;
            break;
        }
        idx++;
    }
    _shiftLeftArray(idx);

    uint8_t len = 2;
    while ((_data.buffer[len - 1] & 0b10000000) == 0) {
        len++;
        if (len == BUFFER_SIZE) {
            error = 2;
            break;
        }
    }

    if ((_data.buffer[len - 1] & 0b01111111) != len) error = 3;

    uint32_t checkSum = 0;
    for (uint8_t i = 1; i < (len - 4); i++) {
        checkSum += _data.buffer[i];
    }
    if (checkSum != (uint32_t)((_data.buffer[len - 4] << 14) | (_data.buffer[len - 3] << 7) | _data.buffer[len - 2])) error = 4;
    
    if (error == 0) {
        for (uint8_t i = 0; i < len; i++) {
            _data.toRead[i] = _data.buffer[i];
        }
    }

    _shiftLeftArray(len);
    return error;
}

void dataDecoder::_shiftLeftArray(uint8_t step_) {
    for (uint8_t i = 0; i < BUFFER_SIZE; i++) {
        if ((i + step_) < BUFFER_SIZE) _data.buffer[i] = _data.buffer[i + step_];
        else _data.buffer[i] = 0;
    }
    if (_bufferIndex >= step_) _bufferIndex -= step_;
    else _bufferIndex = 0;
}

void dataDecoder::_regId(uint8_t id_) {
    uint8_t num = 1;
    while ((_data.toRead[num] >> 7) != 0b1) {
        if ((num % 2) == 1) _type[id_][(num - 1) / 2] = TYPE(_data.toRead[num]);
        else _length[id_].encoded[(num - 2) / 2] = _data.toRead[num];
        num++;
    }
    _length[id_].bitSum = 0;
    for (uint8_t i = 0; i < MAX_DATA_NUM; i++) {
        _length[id_].bitSum += _length[id_].encoded[i];
    }
}

void dataDecoder::_read(uint8_t id_) {
    uint8_t num = 1;
    for (uint16_t i = 0; i < _length[id_].bitSum; i++) {
        _data.binary[i] = ((_data.toRead[num] >> (6 - (i % 7))) & 0b00000001);
        if ((i % 7) == 6) num++;
    }
    if ((_data.toRead[num] >> 7) != 0b1) num++;
}

void dataDecoder::_divide(uint8_t id_) {
    uint16_t num = 0;
    for (uint8_t i = 0; i < MAX_DATA_NUM; i++) {
        _data.dividedBinary[id_][i] = 0;
        uint32_t dividedBinary = 0;
        for (uint8_t j = 0; j < _length[id_].encoded[i]; j++) {
            delayMicroseconds(1);
            dividedBinary = ((dividedBinary << 1) | _data.binary[num]);
            num++;
        }
        if (_type[id_][i] == INT) {
            uint32_t ones = 0;
            bool fill = bitRead(dividedBinary, _length[id_].encoded[i] - 1);
            for (uint8_t j = _length[id_].encoded[i]; j < 32; j++) {
                dividedBinary |= (fill << j);
            }
        }
        _data.dividedBinary[id_][i] = dividedBinary;
    }
}

template <typename T>
T dataDecoder::_cast(uint8_t id_, uint8_t ord_) {
    union {T typed; uint32_t untyped;} data;
    data.untyped = _data.dividedBinary[id_][ord_];
    return data.typed;
}

template bool dataDecoder::_cast(uint8_t id_, uint8_t ord_);
template uint32_t dataDecoder::_cast(uint8_t id_, uint8_t ord_);
template int32_t dataDecoder::_cast(uint8_t id_, uint8_t ord_);
template float dataDecoder::_cast(uint8_t id_, uint8_t ord_);