#pragma once
#include "../net/TcpServer.h"
#include "../net/MessageManager.h"
#include "../net/Packet.h"
#include "../net/PacketType.h"


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
    if (receptionist_id_ == -1)
      return;

    PacketType packet_type;
    auto packet_type_size = packet_type.SerializeFrom(ptr, size);
    if (packet_type_size == 0)
      return;

    auto packet = packet_type.CreatePacket();
    if (!packet)
      return;

    packet->SerializeFromArray(ptr + packet_type_size, size - packet_type_size);
    MessageManager::Get()->PutMessage(-2, receptionist_id_, Receptionist::OnSessionPacket1, session_id, packet_type, packet);
	}
	virtual void OnSessionClosed(int32_t session_id, int32_t reason) {
		if (receptionist_id_ != -1)
			MessageManager::Get()->PutMessage(-2, receptionist_id_, Receptionist::OnSessionClosed, session_id, reason);
	}
private:
	int32_t receptionist_id_;
};