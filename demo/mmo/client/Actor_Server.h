#pragma once
#include <boost/bind.hpp>
#include "../net/Actor2.h"
#include "../common/Packets.pb.h"
#include "UserInfo.h"

class Actor_Server : public Actor {
public:
  Actor_Server() {
    RegisterMsgHandler(-2, boost::bind(&Actor_Server::OnPacket, this, _1, _2));
  }
private:
  void OnPacket(int32_t sender_id, shared_ptr<ActorMsgQ::MsgParams> params) {
    auto mp_packet = dynamic_pointer_cast<MP_Packet>(params);
    if (!mp_packet)
      return;

    auto packet = mp_packet->packet;
    PacketType packet_type(packet);
    if (packet_type == PacketType(make_shared<Packet::SC_LoginResult>())) {
      auto p = dynamic_pointer_cast<Packet::SC_LoginResult>(packet);
      cout << p->welcome() << " " << p->name() << endl;
      UserInfo::Get()->SetName(p->name());
    } else if (packet_type == PacketType(make_shared<Packet::SC_SomeoneSay>())) {
      auto p = dynamic_pointer_cast<Packet::SC_SomeoneSay>(packet);
      cout << p->name() << ": " << p->text() << endl;
    }
  }
};