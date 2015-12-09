#include "stdafx.h"
#include "Broker.h"
#include <regex>
#include "Receptionist.h"

void Broker::OnPacket(shared_ptr<Actor> actor, const string &s)
{
	auto self = dynamic_pointer_cast<Broker>(actor);
	if (!self)
		return;

	cout << "[ActorId = " << self->actor_id_ << "]Broker::OnPacket(" << s << ")" << endl;
	smatch sm;
	regex_match(s, sm, regex("(\\*|\\d+):(.*)"));
	if (sm.size() == 0) {
		cout << s << endl;
	} else {
		if (sm[1] == "*") {
			MessageManager::Get()->PutMessage(self->actor_id_, self->receptionist_id_, Receptionist::SendToAllSessions, self->actor_id_, string(sm[2]));
		} else {
			MessageManager::Get()->PutMessage(self->actor_id_, self->receptionist_id_, Receptionist::SendToSession, self->actor_id_, stoi(sm[1]), string(sm[2]));
		}
	}
}