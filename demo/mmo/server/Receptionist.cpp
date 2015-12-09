#include "stdafx.h"
#include "Receptionist.h"
#include "../net/TcpServer.h"

void Receptionist::SendToSession(shared_ptr<Actor> actor, int32_t sender_actor_id, int32_t receiver_actor_id, const string &s)
{
	auto self = dynamic_pointer_cast<Receptionist>(actor);
	if (!self)
		return;

	auto it = self->broker_to_session_.find(receiver_actor_id);
	if (it != self->broker_to_session_.end())
		self->tcp_server_->PutCmd(unique_ptr<string>(new string(to_string(it->second) + ":" + to_string(sender_actor_id) + ":" + s)));
}

void Receptionist::SendToAllSessions(shared_ptr<Actor> actor, int32_t sender_actor_id, const string &s)
{
	auto self = dynamic_pointer_cast<Receptionist>(actor);
	if (!self)
		return;

	for (const auto &item : self->broker_to_session_) {
		if (item.first == sender_actor_id)
			continue;

		self->tcp_server_->PutCmd(unique_ptr<string>(new string(to_string(item.second) + ":" + to_string(sender_actor_id) + ":" + s)));
	}


}