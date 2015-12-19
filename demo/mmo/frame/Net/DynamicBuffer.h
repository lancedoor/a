#pragma once

class DynamicBuffer {
public:
  DynamicBuffer(int32_t min_size, int32_t max_size) {
    min_size_ = min_size;
    max_size_ = max_size;
    buf_size_ = min_size;
    buf_ptr_ = shared_ptr<uint8_t>(new uint8_t[buf_size_]);
  }
  uint8_t* GetBufPtr() {
    return buf_ptr_.get();
  }

  bool MakeRoom(int32_t required_size, int32_t copy_size) {
    if (required_size > max_size_)
      return false;
    if (required_size > buf_size_) {
      int32_t new_buf_size = buf_size_ * 2;
      while (new_buf_size < required_size)
        new_buf_size *= 2;
      Resize(new_buf_size, copy_size);
    }
    return true;
  }
  void SaveRoom(int32_t required_size, int32_t copy_size) {
    if (required_size < buf_size_ / 4 && buf_size_ / 2 >= min_size_) {
      Resize(buf_size_ / 2, copy_size);
    }
  }

private:
  void Resize(int32_t buf_size, int32_t copy_size) {
    assert(buf_size >= copy_size);
    assert(buf_size >= min_size_ && buf_size <= max_size_);
    if (buf_size < min_size_ || buf_size > max_size_)
      return;
    if (buf_size == buf_size_)
      return;

    auto new_ptr = shared_ptr<uint8_t>(new uint8_t[buf_size]);
    if (buf_ptr_ && copy_size > 0)
      memcpy(new_ptr.get(), buf_ptr_.get(), min(copy_size, buf_size));
    buf_ptr_ = new_ptr;
    buf_size_ = buf_size;
  }
private:
  int32_t min_size_;
  int32_t max_size_;
  shared_ptr<uint8_t> buf_ptr_;
  int32_t buf_size_;
};
