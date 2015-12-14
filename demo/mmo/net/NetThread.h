#pragma once
#include "Thread.h"
#include "NetThreadMsg.h"
#include "MutexPtrDeque.h"

class NetThread : public Thread {
public:
  NetThread(){
	}
	~NetThread() {
	}
protected:
  MutexPtrDeque<NetThreadMsg> msg_q_;
};
