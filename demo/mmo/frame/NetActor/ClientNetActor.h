#pragma once
#include "../Actor/Actor.h"
#include "MsgParamDefine.h"

// SendPacket:
//  MessageId

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
  //void OnNetMsg(int32_t sender_id, shared_ptr<ActorMsgParam> params) {
  //  auto mp_net = dynamic_pointer_cast<MP_Net>(params);
  //  if (!mp_net)
  //    return;

  //  switch (mp_net->msg_type) {
  //  case MP_Net::EMsgType::SEND:
  //    SendPacket(sender_id, mp_net);
  //    break;
  //  case MP_Net::EMsgType::ON_PACKET:
  //    OnPacket(sender_id, mp_net);
  //    break;
  //  }
  //}
  //void SendPacket(int32_t /*sender_id*/, shared_ptr<MP_Net> mp_net) {
  //  if (!net_thread_)
  //    return;
  //  auto msg = make_shared<NetThreadMsg>(NetThreadMsg::SEND, 0, mp_net->packet);
  //  net_thread_->PostMsg(msg);
  //}
  //void OnPacket(int32_t sender_id, shared_ptr<MP_Net> mp_net) {
  //  PacketType packet_type(mp_net->packet);
  //  auto it = packet_handlers_.find(packet_type);
  //  if (it != packet_handlers_.end()) {
  //    it->second(mp_net->packet);
  //  }
  //}
  //void OnClose(int32_t sender_id, shared_ptr<ActorMsgParam> params) {
  //}
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
  //shared_ptr<NetThread> net_thread_;
  unordered_map<PacketType, PacketHandler> packet_handlers_;
};
