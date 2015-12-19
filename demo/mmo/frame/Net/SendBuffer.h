#pragma once

class SendBuffer : public DynamicBuffer {
public:
  SendBuffer(int32_t min_size, int32_t max_size)
  : DynamicBuffer(min_size, max_size) {
    data_size_ = 0;
  }
  int32_t GetDataSize() {
    return data_size_;
  }

  bool Append(uint8_t *ptr, int32_t size) {
    if (!MakeRoom(size + data_size_, data_size_))
      return false;

    memcpy(GetBufPtr() + data_size_, ptr, size);
    data_size_ += size;
    return true;
  }
  void Clear() {
    SaveRoom(data_size_, 0);
    data_size_ = 0;
  }
private:
  int32_t data_size_;
};
