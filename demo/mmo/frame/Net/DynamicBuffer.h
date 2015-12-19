#pragma once

class DynamicBuffer {
  enum {
    kMinCapacity = 4 * 1024,
    kMaxCapacity = 8 * 1024 * 1024,
  };
public:
  DynamicBuffer() {
    data_ptr_ = nullptr;
    data_capacity_ = 0;
    data_size_ = 0;
    ResizeCapacity(kMinCapacity);
  }
  uint8_t* GetDataPtr() {
    return data_ptr_.get();
  }
  int32_t GetDataSize() {
    return data_size_;
  }

  bool Append(uint8_t *ptr, int32_t size) {
    int32_t expect_size = size + data_size_;
    if (expect_size > kMaxCapacity)
      return false;
    if (expect_size > data_capacity_) {
      int32_t new_capacity = data_capacity_;
      while (expect_size > new_capacity)
        new_capacity *= 2;
      ResizeCapacity(new_capacity);
    }

    memcpy(data_ptr_.get() + data_size_, ptr, size);
    data_size_ += size;
    return true;
  }
  void Clear() {
    int32_t old_data_size = data_size_;
    data_size_ = 0;
    if (old_data_size < data_capacity_ / 4 && data_capacity_ / 2 >= kMinCapacity) {
      ResizeCapacity(data_capacity_ / 2);
    }
  }

private:
  void ResizeCapacity(int32_t new_capacity) {
    assert(new_capacity >= kMinCapacity && new_capacity <= kMaxCapacity);
    assert(new_capacity >= data_size_);
    if (new_capacity < data_size_)
      return;
    if (new_capacity == data_capacity_)
      return;

    auto new_ptr = shared_ptr<uint8_t>(new uint8_t[new_capacity]);
    if (data_ptr_ && data_size_ > 0)
      memcpy(new_ptr.get(), data_ptr_.get(), min(data_size_, new_capacity));
    data_ptr_ = new_ptr;
    data_capacity_ = new_capacity;
  }
private:
  shared_ptr<uint8_t> data_ptr_;
  int32_t data_capacity_;
  int32_t data_size_;
};
