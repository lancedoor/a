#pragma once
#include "../net/TcpServer.h"
#include "../net/MessageManager.h"

class MyTcpServer : public TcpServer {
public:
	MyTcpServer(int32_t receptionist_id) {
		receptionist_id_ = receptionist_id;
	}
private:
	virtual void OnNewSession(int32_t session_id) {
		MessageManager::Get()->PutMessage(-2, receptionist_id_, Receptionist::OnNewSession, session_id);
	}
	virtual void OnSessionPacket(int32_t session_id, const string &s) {
		MessageManager::Get()->PutMessage(-2, receptionist_id_, Receptionist::OnSessionPacket, session_id, s);
	}
	virtual void OnSessionClosed(int32_t session_id, int32_t reason) {
		MessageManager::Get()->PutMessage(-2, receptionist_id_, Receptionist::OnSessionClosed, session_id, reason);
	}
private:
	int32_t receptionist_id_;
};