#pragma once
#include "Actor.h"
#include "MsgParamsDefine.h"

class ClientNetActor : public Actor {
  typedef boost::function<void(shared_ptr<::google::protobuf::Message>)> PacketHandler;
public:
  ClientNetActor(int32_t msg_id_packet, int32_t msg_id_close) {
    RegisterMsgHandler(msg_id_packet, boost::bind(&ClientNetActor::OnPacket, this, _1, _2));
    RegisterMsgHandler(msg_id_close, boost::bind(&ClientNetActor::OnClose, this, _1, _2));
  }
private:
  void OnPacket(int32_t sender_id, shared_ptr<ActorMsgQ::MsgParams> params) {
    auto mp_packet = dynamic_pointer_cast<MP_Packet>(params);
    if (!mp_packet)
      return;

    PacketType packet_type(mp_packet->packet);
    auto it = packet_handlers_.find(packet_type);
    if (it != packet_handlers_.end()) {
      it->second(mp_packet->packet);
    }
  }
  void OnClose(int32_t sender_id, shared_ptr<ActorMsgQ::MsgParams> params) {
  }
  void RegisterPacketHandler(const PacketType &packet_type, const PacketHandler &handler) {
    packet_handlers_[packet_type] = handler;
  }
private:
  unordered_map<PacketType, PacketHandler> packet_handlers_;
};
