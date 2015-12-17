#pragma once
#include <mutex>
using namespace std;

#include "../frame/Util/Singleton.h"

class UserInfo {
  DECLARE_SINGLETON(UserInfo)
public:
  void SetName(const string &s) {
    lock_guard<mutex> lg(mutex_);
    name_ = s;
  }
  string GetName() {
    lock_guard<mutex> lg(mutex_);
    return name_;
  }
private:
  mutex mutex_;
  string name_;
};