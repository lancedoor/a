#pragma once
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <mutex>
using namespace std;

#include "ActorMsgParam.h"

class ActorMsgQ {
public:
  struct Msg {
    int32_t sender;
    int32_t receiver;
    int32_t msg_id;
    shared_ptr<ActorMsgParam> params;
  };

  typedef deque<shared_ptr<Msg>> MsgQ;
public:
  void PostMsg(int32_t sender, int32_t receiver, int32_t msg_id, shared_ptr<ActorMsgParam> params) {
    auto msg = make_shared<Msg>();
    msg->sender = sender;
    msg->receiver = receiver;
    msg->msg_id = msg_id;
    msg->params = params;
    PostMsg(msg);
  }
  void PostMsg(shared_ptr<Msg> msg) {
    if (!msg)
      return;

    lock_guard<mutex> lg(mutex_);
    auto it = queues_.find(msg->receiver);
    if (it == queues_.end()) {
      auto q = make_shared<MsgQ>();
      q->push_back(msg);
      queues_[msg->receiver] = q;
    } else {
      it->second->push_back(msg);
    }
  }

	shared_ptr<MsgQ> StartMessageQueueProcess() {
		shared_ptr<MsgQ> ret = nullptr;
		lock_guard<mutex> lg(mutex_);
		for (auto it = queues_.begin(); it != queues_.end(); ++it) {
			if (processing_receivers_.find(it->first) != processing_receivers_.end())
				continue;

      processing_receivers_.insert(it->first);
			ret = it->second;
			queues_.erase(it);
			break;
		}
    
		return ret;
	}
	void EndMessageQueueProcess(int32_t receiver_id) {
		lock_guard<mutex> lg(mutex_);
		processing_receivers_.erase(receiver_id);
	}
private:
private:
	mutex mutex_;
	unordered_map<int32_t, shared_ptr<MsgQ>> queues_;
	unordered_set<int32_t> processing_receivers_;
};
