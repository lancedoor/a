#pragma once
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <mutex>
#include <boost/function.hpp>
#include "Singleton.h"
using namespace std;

class Actor;
struct Message {
	int32_t sender;
	int32_t receiver;
	boost::function<void(shared_ptr<Actor>)> fn;
};

typedef deque<shared_ptr<Message>> MessageQueue;

class MessageManager {
	DECLARE_SINGLETON(MessageManager)
public:
	template<typename Fn, typename... Params>
	void PutMessage(int sender_id, int32_t receiver_id, Fn fn, Params... params) {
		auto msg = make_shared<Message>();
		msg->sender = sender_id;
		msg->receiver = receiver_id;
		msg->fn = boost::bind(fn, _1, params...);
		MessageManager::Get()->PutMessage(msg);
	}

	shared_ptr<MessageQueue> GetMessageQueue(int32_t receiver_id) {
		shared_ptr<MessageQueue> ret;
		lock_guard<mutex> lg(mutex_);
		auto it = queues_.find(receiver_id);
		if (it == queues_.end()) {
			ret = nullptr;
		} else {
			ret = it->second;
			queues_.erase(it);
		}
		return ret;
	}
	shared_ptr<MessageQueue> StartMessageQueueProcess() {
		shared_ptr<MessageQueue> ret = nullptr;
		lock_guard<mutex> lg(mutex_);
		for (auto it = queues_.begin(); it != queues_.end(); ++it) {
			if (processing_receivers_.find(it->first) != processing_receivers_.end())
				continue;
			if (it->first < 2)
				continue;

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
	void PutMessage(shared_ptr<Message> message) {
		if (!message)
			return;

		lock_guard<mutex> lg(mutex_);
		auto it = queues_.find(message->receiver);
		if (it == queues_.end()) {
			auto q = make_shared<MessageQueue>();
			q->push_back(message);
			queues_[message->receiver] = q;
		} else {
			it->second->push_back(message);
		}
	}
private:
	mutex mutex_;
	unordered_map<int32_t, shared_ptr<MessageQueue>> queues_;
	unordered_set<int32_t> processing_receivers_;
};
