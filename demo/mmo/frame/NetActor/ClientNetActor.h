#pragma once
#include <boost/bind.hpp>
#include "../Actor/Actor.h"
#include "../Net/PacketType.h"
#include "MsgParamDefine.h"
#include "NamedMsgId.h"

class ClientNetActor : public Actor {
  typedef boost::function<void(shared_ptr<::google::protobuf::Message>)> PacketHandler;
public:
  ClientNetActor() {
    RegisterMsgHandler(NamedMsgId::Get()->GetMsgId("core::net::on_packet"), boost::bind(&ClientNetActor::OnMsg_Packet, this, _1, _2));
  }
protected:
  void RegisterPacketHandler(const PacketType &packet_type, const PacketHandler &handler) {
    packet_handlers_[packet_type] = handler;
  }
private:
  void OnMsg_Packet(int32_t sender_id, shared_ptr<ActorMsgParam> param) {
    auto  mp_packet = dynamic_pointer_cast<MP_Packet>(param);
    if (!mp_packet)
      return;

    PacketType packet_type(mp_packet->packet);
    auto it = packet_handlers_.find(packet_type);
    if (it != packet_handlers_.end()) {
      it->second(mp_packet->packet);
    }
  }
private:
  unordered_map<PacketType, PacketHandler> packet_handlers_;
};
