#pragma once
#include "../net/Actor.h"

class Broker : public Actor {
public:
	Broker() {

	}
	static void OnPacket(shared_ptr<Actor> actor, const string &s) {
		auto self = dynamic_pointer_cast<Broker>(actor);
		if (!self)
			return;

		cout << "[ActorId = " << self->actor_id_ << "]Broker::OnPacket(" << s << ")" << endl;
	}
	static void OnClosed(shared_ptr<Actor> actor, int32_t reason) {
		auto self = dynamic_pointer_cast<Broker>(actor);
		if (!self)
			return;

		cout << "[ActorId = " << self->actor_id_ << "]Broker::OnClosed(" << reason << ")" << endl;
	}
};