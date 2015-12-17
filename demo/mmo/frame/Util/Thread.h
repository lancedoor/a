#pragma once
#include <thread>
using namespace std;

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
protected:
	bool stop_;
private:
	virtual void ThreadProc() = 0 {
	}
};
