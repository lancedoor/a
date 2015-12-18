#pragma once
#include "IdMap.h"

template<typename ItemClass>
class IdPtrMap : public IdMap<shared_ptr<ItemClass>> {
public:
  IdPtrMap(int32_t item_capacity = INT32_MAX)
  : IdMap(item_capacity) {
  }

  shared_ptr<ItemClass> GetItem(int32_t id) {
    auto it = find(id);
    return it == end() ? nullptr : it->second;
  }
};