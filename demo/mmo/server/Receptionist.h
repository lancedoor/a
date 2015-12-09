#pragma once
#include "../net/Actor.h"
#include "../net/ActorManager.h"
#include "Broker.h"

class Receptionist : public Actor {
public:
	Receptionist() {

	}
	static void OnNewSession(shared_ptr<Actor> actor, int32_t session_id) {
		auto self = dynamic_pointer_cast<Receptionist>(actor);
		if (!self)
			return;

		cout << "[ActorId = " << self->actor_id_ << "]Receptionist::OnNewSession(" << session_id << ")" << endl;
		int32_t broker_id = ActorManager::Get()->AddActor(make_shared<Broker>());
		self->session_to_broker_[session_id] = broker_id;
	}
	static void OnSessionPacket(shared_ptr<Actor> actor, int32_t session_id, const string &s) {
		auto self = dynamic_pointer_cast<Receptionist>(actor);
		if (!self)
			return;

		cout << "[ActorId = " << self->actor_id_ << "]Receptionist::OnSessionPacket(" << session_id << ", " << s << ")" << endl;
		auto it = self->session_to_broker_.find(session_id);
		if (it != self->session_to_broker_.end())
			MessageManager::Get()->PutMessage(self->actor_id_, it->second, Broker::OnPacket, s);
	}
	static void OnSessionClosed(shared_ptr<Actor> actor, int32_t session_id, int32_t reason) {
		auto self = dynamic_pointer_cast<Receptionist>(actor);
		if (!self)
			return;

		cout << "[ActorId = " << self->actor_id_ << "]Receptionist::OnSessionClosed(" << session_id << ", " << reason << ")" << endl;
		auto it = self->session_to_broker_.find(session_id);
		if (it != self->session_to_broker_.end()) {
			MessageManager::Get()->PutMessage(self->actor_id_, it->second, Broker::OnClosed, reason);
			self->session_to_broker_.erase(it);
		}
	}
private:
	unordered_map<int32_t, int32_t> session_to_broker_;
};