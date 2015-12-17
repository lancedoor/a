#pragma once
#include "../Util/Thread.h"
#include "../Util/MutexPtrDeque.h"
#include "NetThreadMsg.h"

class NetThread : public Thread {
public:
  NetThread(){
	}
	~NetThread() {
	}
  void PostMsg(shared_ptr<NetThreadMsg> msg) {
    msg_q_.PushBack(msg);
  }
protected:
  MutexPtrDeque<NetThreadMsg> msg_q_;
};
