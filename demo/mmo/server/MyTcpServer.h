#pragma once
#include "../net/TcpServer.h"
#include "../net/MessageManager.h"


class MyTcpServer : public TcpServer {
public:
	MyTcpServer()
	: receptionist_id_(-1) {
		
	}
	void SetReceptionistId(int32_t receptionist_id) {
		receptionist_id_ = receptionist_id;
	}
private:
	virtual void OnNewSession(int32_t session_id) {
		if (receptionist_id_ != -1)
			MessageManager::Get()->PutMessage(-2, receptionist_id_, Receptionist::OnNewSession, session_id);
	}
  virtual void OnSessionPacket(int32_t session_id, shared_ptr<::google::protobuf::Message> packet) {
    if (receptionist_id_ == -1)
      return;
    MessageManager::Get()->PutMessage(-2, receptionist_id_, Receptionist::OnSessionPacket, session_id, packet);
  }
	virtual void OnSessionClosed(int32_t session_id, int32_t reason) {
		if (receptionist_id_ != -1)
			MessageManager::Get()->PutMessage(-2, receptionist_id_, Receptionist::OnSessionClosed, session_id, reason);
	}
private:
	int32_t receptionist_id_;
};