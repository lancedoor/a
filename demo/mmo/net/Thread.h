#pragma once
#include <thread>
using namespace std;
#include "CmdQ.h"

class Thread : public thread {
public:
	Thread(){
	}
	~Thread() {
	}
	virtual void Start() {
		stop_ = false;
		swap(thread(&Thread::ThreadProc, this));
	}
	virtual void Stop() {
		stop_ = true;
	}
  void PutCmd(shared_ptr<Cmd> cmd) {
    cmd_q_.PutCmd(move(cmd));
  }
protected:
  shared_ptr<Cmd> GetCmd() {
    return cmd_q_.GetCmd();
  }

	bool stop_;
private:
	virtual void ThreadProc() = 0 {
	}
private:
  CmdQ cmd_q_;
};
