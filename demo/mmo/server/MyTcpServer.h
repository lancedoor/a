#pragma once
#include "../net/TcpServer.h"
#include "../net/MessageManager.h"
#include "../net/Packet.h"

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
	virtual void OnSessionPacket(int32_t session_id, uint8_t *ptr, uint32_t size) {
    int32_t packet_type = *((int32_t*)ptr);

    shared_ptr<Packet> packet;
    switch (packet_type) {
    case CS_LOGIN: packet = make_shared<CS_Login>(); break;
    case CS_CHAT: packet = make_shared<CS_Chat>(); break;
    }
    
    if (!packet)
      return;

    packet->SerializeFromArray(ptr + 4, size - 4);

    if (receptionist_id_ != -1)
			MessageManager::Get()->PutMessage(-2, receptionist_id_, Receptionist::OnSessionPacket1, session_id, packet_type, packet);
	}
	virtual void OnSessionClosed(int32_t session_id, int32_t reason) {
		if (receptionist_id_ != -1)
			MessageManager::Get()->PutMessage(-2, receptionist_id_, Receptionist::OnSessionClosed, session_id, reason);
	}
private:
	int32_t receptionist_id_;
};