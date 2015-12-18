#pragma once
#include <unordered_map>

template<typename ItemClass>
class IdMap : public unordered_map<int32_t, ItemClass> {
public:
  IdMap(int32_t item_capacity = INT32_MAX) {
    item_capacity_ = item_capacity;
    next_id_ = 0;
  }
  bool IsFull() {
    return (int32_t)size() >= item_capacity_;
  }

  int32_t AddItem(const ItemClass &item) {
    if (IsFull())
      return -1;
    while (find(next_id_) != end()) {
      ++next_id_;
    }
    int32_t item_id = next_id_;
    next_id_ = next_id_ == INT32_MAX ? 0 : next_id_ + 1;
    insert({ item_id, item });
    return item_id;
  }
private:
  int32_t item_capacity_;
  int32_t next_id_;
};