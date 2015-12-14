#pragma once
#include "../net/Actor.h"
#include "../net/Packet.h"
#include "../net/PacketType.h"
#include "../common/Packets.pb.h"

class Broker : public Actor {
public:
	Broker(int32_t receptionist_id)
		: receptionist_id_(receptionist_id) {
    packet_handlers_[PacketType(make_shared<Packet::CS_Login>())] = boost::bind(&Broker::OnPacket_CS_Login2, this, _1);
    packet_handlers_[PacketType(make_shared<Packet::CS_Say>())] = boost::bind(&Broker::OnPacket_CS_Chat2, this, _1);
	}
	static void OnStart(shared_ptr<Actor> actor);
	//static void OnPacket(shared_ptr<Actor> actor, const string &s);
  static void OnPacket_CS_Login(shared_ptr<Actor> actor, shared_ptr<Packet::CS_Login> packet);
  static void OnPacket_CS_Chat(shared_ptr<Actor> actor, shared_ptr<Packet::CS_Say> packet);

  void OnPacket_CS_Login2(shared_ptr<::google::protobuf::Message> _packet) {
    auto packet = dynamic_pointer_cast<Packet::CS_Login>(_packet);
    if (!packet)
      return;
    cout << "Broker::OnPacket_CS_Login2(user=" << packet->user() << ")" << endl;
  }
  void OnPacket_CS_Chat2(shared_ptr<::google::protobuf::Message> _packet) {
    auto packet = dynamic_pointer_cast<Packet::CS_Say>(_packet);
    if (!packet)
      return;
    cout << "Broker::OnPacket_CS_Chat2(text=" << packet->text() << ")" << endl;
  }

  static void OnClosed(shared_ptr<Actor> actor, int32_t reason) {
		auto self = dynamic_pointer_cast<Broker>(actor);
		if (!self)
			return;

		cout << "[ActorId = " << self->actor_id_ << "]Broker::OnClosed(" << reason << ")" << endl;
	}

  static void OnPacket(shared_ptr<Actor> actor, shared_ptr<::google::protobuf::Message> packet) {
    auto self = dynamic_pointer_cast<Broker>(actor);
    if (!self)
      return;

    PacketType packet_type(packet);
    auto it = self->packet_handlers_.find(packet_type);
    if (it != self->packet_handlers_.end()) {
      it->second(packet);
    }
  }
private:
	void SendToClient(const string &s);
private:
	int32_t receptionist_id_;
  unordered_map<PacketType, boost::function<void(shared_ptr<::google::protobuf::Message>)>> packet_handlers_;
};