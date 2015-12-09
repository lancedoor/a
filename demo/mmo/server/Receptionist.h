#pragma once

class Receptionist : public Actor {
public:
	Receptionist() {

	}
	static void OnNewSession(shared_ptr<Actor> actor, int32_t session_id) {
		auto self = dynamic_pointer_cast<Receptionist>(actor);
		if (!self)
			return;

		cout << "[ActorId = " << self->actor_id_ << "]Receptionist::OnNewSession(" << session_id << ")" << endl;
	}
	static void OnSessionPacket(shared_ptr<Actor> actor, int32_t session_id, const string &s) {
		auto self = dynamic_pointer_cast<Receptionist>(actor);
		if (!self)
			return;

		cout << "[ActorId = " << self->actor_id_ << "]Receptionist::OnSessionPacket(" << session_id << ", " << s << ")" << endl;
	}
	static void OnSessionClosed(shared_ptr<Actor> actor, int32_t session_id, int32_t reason) {
		auto self = dynamic_pointer_cast<Receptionist>(actor);
		if (!self)
			return;

		cout << "[ActorId = " << self->actor_id_ << "]Receptionist::OnSessionClosed(" << session_id << ", " << reason << ")" << endl;
	}

};