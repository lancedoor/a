#pragma once
#include <boost/bind.hpp>
#include "../Actor/Actor.h"
#include "NamedMsgId.h"
#include "MsgParamDefine.h"

class ServerNetActor : public Actor {
  typedef boost::function<void(int32_t, shared_ptr<::google::protobuf::Message>)> PacketHandler;
public:
  ServerNetActor() {
    RegisterMsgHandler(NamedMsgId::Get()->GetMsgId("core::net::on_session_packet"), boost::bind(&ServerNetActor::OnMsg_SessionPacket, this, _1, _2));
  }
protected:
  void RegisterPacketHandler(const PacketType &packet_type, const PacketHandler &handler) {
    packet_handlers_[packet_type] = handler;
  }
private:
  void OnMsg_SessionPacket(int32_t sender_id, shared_ptr<ActorMsgParam> param) {
    auto  mp_session_packet = dynamic_pointer_cast<MP_SessionPacket>(param);
    if (!mp_session_packet)
      return;

    PacketType packet_type(mp_session_packet->packet);
    auto it = packet_handlers_.find(packet_type);
    if (it != packet_handlers_.end()) {
      it->second(mp_session_packet->session_id, mp_session_packet->packet);
    }
  }
private:
  unordered_map<PacketType, PacketHandler> packet_handlers_;
};
