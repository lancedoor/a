#pragma once
#include <mutex>
#include <deque>

// Command
struct Cmd {
  int32_t id;
  int32_t i_param;
  shared_ptr<uint8_t> p_param;
  int32_t p_size;

  Cmd(int32_t _id, int32_t _i_param, shared_ptr<uint8_t> _p_param, int32_t _p_size) {
    id = _id;
    i_param = _i_param;
    p_param = _p_param;
    p_size = _p_size;
  }
};

// CommandQueue
class CmdQ {
public:
	void PutCmd(shared_ptr<Cmd> cmd) {
		lock_guard<mutex> lg(mutex_);
		cmds_.push_back(cmd);
	}
	shared_ptr<Cmd> GetCmd() {
		lock_guard<mutex> lg(mutex_);
		if (cmds_.size() == 0)
			return nullptr;
		auto ret = cmds_.front();
		cmds_.pop_front();
		return ret;
	}
private:
	mutex mutex_;
	deque<shared_ptr<Cmd>> cmds_;
};
