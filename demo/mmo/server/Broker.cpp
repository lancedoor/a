#include "stdafx.h"
#include "Broker.h"
#include <regex>
#include "Receptionist.h"


//void Broker::OnPacket(shared_ptr<Actor> actor, const string &s)
//{
//	auto self = dynamic_pointer_cast<Broker>(actor);
//	if (!self)
//		return;
//
//
//
//	cout << "[ActorId = " << self->actor_id_ << "]Broker::OnPacket(" << s << ")" << endl;
//	smatch sm;
//	regex_match(s, sm, regex("(\\*|\\d+):(.*)"));
//	if (sm.size() == 0) {
//		cout << s << endl;
//	} else {
//		if (sm[1] == "*") {
//			MessageManager::Get()->PutMessage(self->actor_id_, self->receptionist_id_, Receptionist::SendToAllSessions, self->actor_id_, string(sm[2]));
//		} else {
//			MessageManager::Get()->PutMessage(self->actor_id_, self->receptionist_id_, Receptionist::SendToSession, self->actor_id_, stoi(sm[1]), string(sm[2]));
//		}
//	}
//}

void Broker::OnPacket_CS_Login(shared_ptr<::google::protobuf::Message> _packet)
{
  auto packet = dynamic_pointer_cast<Packet::CS_Login>(_packet);
  if (!packet)
    return;
  cout << "Broker::OnPacket<CS_Login>(user=" << packet->user() << ")" << endl;

  name_ = "guest" + to_string(actor_id_);
  auto sc_packet = make_shared<Packet::SC_LoginResult>();
  sc_packet->set_name(name_);
  sc_packet->set_welcome("welcome");
  SendToClient(sc_packet);
}

void Broker::OnPacket_CS_Say(shared_ptr<::google::protobuf::Message> _packet)
{
  auto packet = dynamic_pointer_cast<Packet::CS_Say>(_packet);
  if (!packet)
    return;

  cout << name_ + ":" + packet->text() << endl;

  auto sc_packet = make_shared<Packet::SC_SomeoneSay>();
  sc_packet->set_name(name_);
  sc_packet->set_text(packet->text());

  MessageManager::Get()->PutMessage(actor_id_, receptionist_id_, Receptionist::SendToAllSessions, actor_id_, sc_packet);
}

void Broker::SendToClient(shared_ptr<::google::protobuf::Message> packet)
{
	PutMessage(receptionist_id_, Receptionist::SendToSession, actor_id_, actor_id_, packet);
}
