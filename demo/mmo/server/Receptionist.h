#pragma once
#include "../net/Actor.h"
#include "../net/ActorManager.h"
#include "Broker.h"
#include "../net/PacketType.h"

class TcpServer;
class Receptionist : public Actor {
public:
	Receptionist(shared_ptr<TcpServer> tcp_server)
	: tcp_server_(tcp_server) {

	}
	static void OnNewSession(shared_ptr<Actor> actor, int32_t session_id) {
		auto self = dynamic_pointer_cast<Receptionist>(actor);
		if (!self)
			return;

		cout << "[ActorId = " << self->actor_id_ << "]Receptionist::OnNewSession(" << session_id << ")" << endl;
		int32_t broker_id = ActorManager::Get()->AddActor(make_shared<Broker>(self->actor_id_));
		self->session_to_broker_[session_id] = broker_id;
		self->broker_to_session_[broker_id] = session_id;

		self->PutMessage(broker_id, Broker::OnStart);
	}
	//static void OnSessionPacket(shared_ptr<Actor> actor, int32_t session_id, uint8_t *ptr, uint32_t size) {
	//	auto self = dynamic_pointer_cast<Receptionist>(actor);
	//	if (!self)
	//		return;

 //   string s((const char*)ptr, size);
 //   cout << "[ActorId = " << self->actor_id_ << "]Receptionist::OnSessionPacket(" << session_id << ", " << s << ")" << endl;
	//	auto it = self->session_to_broker_.find(session_id);
 //   if (it == self->session_to_broker_.end())
 //     return;

 //   self->PutMessage(it->second, Broker::OnPacket, s);
	//}
  static void OnSessionPacket(shared_ptr<Actor> actor, int32_t session_id, shared_ptr<::google::protobuf::Message> packet) {
    auto self = dynamic_pointer_cast<Receptionist>(actor);
    if (!self)
      return;

    auto it = self->session_to_broker_.find(session_id);
    if (it == self->session_to_broker_.end())
      return;

    self->PutMessage(it->second, Broker::OnPacket, packet);

    //switch (PacketType(packet).type_) {
    //case CS_LOGIN: {
    //  self->PutMessage(it->second, Broker::OnPacket_CS_Login, dynamic_pointer_cast<CS_Login>(packet));
    //  break;
    //}
    //case CS_CHAT: {
    //  self->PutMessage(it->second, Broker::OnPacket_CS_Chat, dynamic_pointer_cast<CS_Chat>(packet));
    //  break;
    //}

    //}


    

  }
  static void OnSessionClosed(shared_ptr<Actor> actor, int32_t session_id, int32_t reason) {
		auto self = dynamic_pointer_cast<Receptionist>(actor);
		if (!self)
			return;

		cout << "[ActorId = " << self->actor_id_ << "]Receptionist::OnSessionClosed(" << session_id << ", " << reason << ")" << endl;
		auto it = self->session_to_broker_.find(session_id);
		if (it != self->session_to_broker_.end()) {
			MessageManager::Get()->PutMessage(self->actor_id_, it->second, Broker::OnClosed, reason);
			self->broker_to_session_.erase(it->second);
			self->session_to_broker_.erase(it);
		}
	}
	static void SendToSession(shared_ptr<Actor> actor, int32_t sender_actor_id, int32_t receiver_actor_id, shared_ptr<::google::protobuf::Message> packet);
	static void SendToAllSessions(shared_ptr<Actor> actor, int32_t sender_actor_id, shared_ptr<::google::protobuf::Message> packet);
private:
	unordered_map<int32_t, int32_t> session_to_broker_;
	unordered_map<int32_t, int32_t> broker_to_session_;
	shared_ptr<TcpServer> tcp_server_;
};