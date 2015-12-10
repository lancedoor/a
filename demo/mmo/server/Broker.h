#pragma once
#include "../net/Actor.h"

class Broker : public Actor {
public:
	Broker(int32_t receptionist_id)
		: receptionist_id_(receptionist_id) {
	}
	static void OnStart(shared_ptr<Actor> actor);
	static void OnPacket(shared_ptr<Actor> actor, const string &s);
	static void OnClosed(shared_ptr<Actor> actor, int32_t reason) {
		auto self = dynamic_pointer_cast<Broker>(actor);
		if (!self)
			return;

		cout << "[ActorId = " << self->actor_id_ << "]Broker::OnClosed(" << reason << ")" << endl;
	}
private:
	void SendToClient(const string &s);
private:
	int32_t receptionist_id_;
};