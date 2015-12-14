#include "stdafx.h"
#include "Receptionist.h"
#include "../net/TcpServer.h"

void Receptionist::SendToSession(shared_ptr<Actor> actor, int32_t sender_actor_id, int32_t receiver_actor_id, shared_ptr<::google::protobuf::Message> packet)
{
	auto self = dynamic_pointer_cast<Receptionist>(actor);
	if (!self)
		return;

	auto it = self->broker_to_session_.find(receiver_actor_id);
  if (it != self->broker_to_session_.end())
    if (sender_actor_id == receiver_actor_id)
      self->tcp_server_->SendPacket(it->second, packet);
		else
      self->tcp_server_->SendPacket(it->second, packet);
}

void Receptionist::SendToAllSessions(shared_ptr<Actor> actor, int32_t sender_actor_id, shared_ptr<::google::protobuf::Message> packet)
{
	auto self = dynamic_pointer_cast<Receptionist>(actor);
	if (!self)
		return;

	for (const auto &item : self->broker_to_session_) {
		if (item.first == sender_actor_id)
			continue;

    self->tcp_server_->SendPacket(item.second, packet);
	}


}