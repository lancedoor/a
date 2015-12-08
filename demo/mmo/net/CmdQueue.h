#pragma once
#include <mutex>
#include <deque>

class CmdQueue {
public:
	void PutCmd(unique_ptr<string> cmd) {
		lock_guard<mutex> lg(cmds_mutex_);
		cmds_.push_back(move(cmd));
	}
	shared_ptr<string> GetCmd() {
		lock_guard<mutex> lg(cmds_mutex_);
		if (cmds_.size() == 0)
			return nullptr;
		auto ret = move(cmds_.front());
		cmds_.pop_front();
		return ret;
	}
private:
	mutex cmds_mutex_;
	deque<unique_ptr<string>> cmds_;
};
