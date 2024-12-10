import struct
import math

class Encoder:
    def __init__(self, id_):
        self.id = id_
        self.isEditable = True
        self.bit_length = 0
        self.byte_length = 0
        self.data = []
        self.order = {}
        self.dType = {}
        self.fType = {}
        self.dSize = {}
        self.data_input = {}
        self.data_binary = []
        self.type_dict = {'bool':1, 'uint':2, 'int':3, 'float':4}
    def append(self, name_, ord_, type_, size_ = 0):
        if self.isEditable == True:
            self.order[name_] = ord_
            self.dType[name_] = self.confirm_dType(type_)
            self.dSize[name_] = self.confirm_dSize(type_, size_)
            self.fType[name_] = self.confirm_fType(name_)
            self.data_input[name_] = 0
        else:
            raise RuntimeError("Cannot edit after init().")
    def init(self):
        if self.isEditable == True:
            self.isEditable = False
            order_sorted = sorted(self.order, key = self.order.get)
            self.dType = {key : self.dType[key] for key in order_sorted}
            self.fType = {key : self.fType[key] for key in order_sorted}
            self.dSize = {key : self.dSize[key] for key in order_sorted}
            self.data_input = {key : self.data_input[key] for key in order_sorted}
            for name_ in self.dSize:
                self.bit_length += self.dSize[name_]
            self.byte_length = math.ceil(self.bit_length / 7) + 2
            self.data.append(0b11000000 | self.id)
            for name_ in self.data_input:
                self.data.append(self.type_dict[self.dType[name_]])
                self.data.append(self.dSize[name_])
            self.data.append(0b11000000 | (len(self.data_input) * 2 + 2))
        else:
            raise RuntimeError("Cannot call init() twice.")
    def encode(self, **kwargs):
        for data_ in kwargs:
            if data_ in self.dType:
                self.data_input[data_] = kwargs[data_]
        self.data_binary.clear()
        self.data.clear()
        self.generate_binary()
        bit_num = 0
        for cnt in range(self.byte_length):
            if cnt == 0:
                data_ = 0b10000000 | self.id
            elif cnt == (self.byte_length - 1):
                data_ = 0b10000000 | self.byte_length
            else:
                data_ = 0
                for cnt_ in range(7):
                    data_ |= self.data_binary[bit_num] << (6 - cnt_)
                    bit_num += 1
                    if bit_num == self.bit_length:
                        break
            self.data.append(data_)
    def confirm_dType(self, type_):
        if not (type_ in self.type_dict):
            raise RuntimeError("Input collect type.")
        return type_
    def confirm_dSize(self, type_, size_):
        if type_ == 'float':
            return 32
        elif type_ == 'bool':
            return 1
        elif size_ == 0:
            return 32
        elif size_ <= 32:
            return size_
        else:
            raise RuntimeError("Input collect size (1~32).")
    def confirm_fType(self, name_):
        if self.dType[name_] == 'bool':
            return '<?'
        if self.dType[name_] == 'uint':
            if self.dSize[name_] <= 8:
                return '<B'
            elif self.dSize[name_] <= 16:
                return '<H'
            else:
                return '<L'
        if self.dType[name_] == 'int':
            if self.dSize[name_] <= 8:
                return '<b'
            elif self.dSize[name_] <= 16:
                return '<h'
            else:
                return '<l'
        if self.dType[name_] == 'float':
            return '<f'
    def get_binary(self, name_):
        return int.from_bytes(struct.pack(self.fType[name_], self.data_input[name_]), byteorder='little')
    def generate_binary(self):
        for name_ in self.data_input:
            for cnt in range(self.dSize[name_]):
                if (cnt == 0 and self.dType[name_] == 'int'):
                    if self.data_input[name_] < 0:
                        self.data_binary.append(1)
                    else:
                        self.data_binary.append(0)
                else:
                    bit = (self.get_binary(name_) >> (self.dSize[name_] - cnt - 1)) & 1
                    if bit == 1:
                        self.data_binary.append(1)
                    else:
                        self.data_binary.append(0)

class Decoder:
    data = []
    data_binary = []
    dataset_dict = {}
    def decode(self):
        if (self.data[0] >> 6) == 0b11:
            self.register_id(self.data[0] & 0b00111111)
        elif (self.data[0] >> 6) == 0b10:
            self.read(self.data[0] & 0b01111111)
        else:
            raise RuntimeError("data receive error")
    def register_id(self, id_):
        dataset_dict_ = {}
        dType = []
        dSize = []
        fType = []
        decoded_data = []
        byte_num = 1
        while (self.data[byte_num] >> 6) == 0 and (self.data[byte_num + 1] >> 6) == 0:
            dType_ = self.data[byte_num]
            dType.append(dType_)
            byte_num += 1
            dSize_ = self.data[byte_num]
            dSize.append(dSize_)
            byte_num += 1
            fType.append(self.confirm_fType(dType_, dSize_))
            decoded_data.append(0)
        bit_length = 0
        for cnt in range(len(dSize)):
            bit_length += dSize[cnt]
        dataset_dict_['dType'] = dType
        dataset_dict_['dSize'] = dSize
        dataset_dict_['fType'] = fType
        dataset_dict_['bit_length'] = bit_length
        dataset_dict_['decoded_data'] = decoded_data
        self.dataset_dict[id_] = dataset_dict_
    def confirm_fType(self, dType_, dSize_):
        if dType_ == 1:
            return '<?'
        if dType_ == 2:
            if dSize_ <= 8:
                return '<B'
            elif dSize_ <= 16:
                return '<H'
            else:
                return '<L'
        if dType_ == 3:
            if dSize_ <= 8:
                return '<b'
            elif dSize_ <= 16:
                return '<h'
            else:
                return '<l'
        if dType_ == 4:
            return '<f'
    def read(self, id_):
        self.data_binary = []
        byte_num = 1
        bit_num = 0
        while (self.data[byte_num] >> 7) == 0:
            for cnt in range(7):
                self.data_binary.append(self.data[byte_num] >> (6 - cnt) & 1)
                bit_num += 1
                if bit_num == self.dataset_dict[id_]['bit_length']:
                    break
            bit_num = 0
            byte_num += 1
        for cnt in range(len(self.dataset_dict[id_]['dSize'])):
            data_ = 0
            for cnt_ in range(self.dataset_dict[id_]['dSize'][cnt]):
                data_ = (data_ << 1) + self.data_binary[bit_num]
                bit_num += 1
            byte_length = math.ceil(self.dataset_dict[id_]['dSize'][cnt] / 8)
            if self.dataset_dict[id_]['dType'][cnt] == 3 and (data_ >> (self.dataset_dict[id_]['dSize'][cnt] - 1)) == 1:
                for cnt_ in range(self.dataset_dict[id_]['dSize'][cnt], byte_length * 8):
                    data_ |= (1 << cnt_)
            self.dataset_dict[id_]['decoded_data'][cnt] = struct.unpack(self.dataset_dict[id_]['fType'][cnt], data_.to_bytes(byte_length, byteorder='little'))[0]