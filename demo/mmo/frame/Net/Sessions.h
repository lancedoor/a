#pragma once
#include <vector>
#include <deque>
#include "google/protobuf/message.h"

template<typename ConnectionType>
class Sessions {
public:
	Sessions(int32_t max_sessions) {
		sessions_.resize(max_sessions);
		for (int32_t i = 0; i < max_sessions; ++i) {
			unused_session_ids_.push_back(i);
		}
	}
	~Sessions() {

	}

	int32_t GetUnusedSessionId() {
		int32_t ret = -1;
		if (unused_session_ids_.size() > 0) {
			ret = unused_session_ids_.front();
			unused_session_ids_.pop_front();
		}
		return ret;
	}
	shared_ptr<ConnectionType> GetSession(int32_t session_id) {
		if (session_id < 0 || session_id >= (int32_t)sessions_.size())
			return nullptr;
		else
			return sessions_[session_id];
	}
	void SetSession(int32_t session_id, shared_ptr<ConnectionType> connection) {
		if (session_id < 0 || session_id >= (int32_t)sessions_.size())
			return;
		assert(sessions_[session_id] == nullptr);
		sessions_[session_id] = connection;
	}
	void ReleaseSession(int32_t session_id) {
		if (session_id < 0 || session_id >= (int32_t)sessions_.size())
			return;
		sessions_[session_id] = nullptr;
		unused_session_ids_.push_back(session_id);
	}

	void CloseAllSessions() {
		for (int32_t i = 0; i < (int32_t)sessions_.size(); ++i) {
			auto conn = sessions_[i];
			if (conn) {
				conn->Close();
				conn = nullptr;
				unused_session_ids_.push_back(i);
			}
		}
	}
  void Broadcast(shared_ptr<::google::protobuf::Message> packet) {
    for (int32_t i = 0; i < (int32_t)sessions_.size(); ++i) {
      auto conn = sessions_[i];
      if (conn) {
        conn->SendPacket(packet);
      }
    }
  }

private:
	vector<shared_ptr<ConnectionType>> sessions_;
	deque<int32_t> unused_session_ids_;
};