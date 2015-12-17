#pragma once
#include <mutex>
#include <unordered_map>
using namespace std;

#include "../Util/Singleton.h"

class NamedMsgId {
  _DECLARE_SINGLETON(NamedMsgId)
public:
  NamedMsgId() {
    next_id_ = -2;
  }
  int32_t GetMsgId(const string &name) {
    int32_t ret = -1;

    lock_guard<mutex> lg(mutex_);
    auto it = named_ids_.find(name);
    if (it == named_ids_.end()) {
      ret = next_id_;
      --next_id_;
      named_ids_[name] = ret;
    } else {
      ret = it->second;
    }

    return ret;
  }
protected:
  mutex mutex_;
  unordered_map<string, int32_t> named_ids_;
  int32_t next_id_;
};
