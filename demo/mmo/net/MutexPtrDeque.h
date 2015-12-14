#pragma once
#include <mutex>
#include <deque>

template<typename ItemType>
class MutexPtrDeque {
public:
  void PushBack(shared_ptr<ItemType> item_ptr) {
    lock_guard<mutex> lg(mutex_);
    items_.push_back(item_ptr);
  }

  shared_ptr<ItemType> PopFront() {
		lock_guard<mutex> lg(mutex_);
		if (items_.size() == 0)
			return nullptr;
		auto ret = items_.front();
    items_.pop_front();
		return ret;
	}
private:
	mutex mutex_;
	deque<shared_ptr<ItemType>> items_;
};
