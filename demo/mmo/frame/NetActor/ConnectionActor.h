#pragma once
#include <boost/bind.hpp>
#include "../Actor/Actor.h"
#include "../Net/PacketType.h"
#include "MsgParamDefine.h"
#include "NamedMsgId.h"

class ConnectionActor : public Actor {
  typedef boost::function<void(shared_ptr<::google::protobuf::Message>)> PacketHandler;
public:
  virtual void Init() {
    Actor::Init();
    REGISTER_MSG_HANDLER(ConnectionActor, NamedMsgId::Get()->GetMsgId("core::net::on_connected"), OnMsg_Connected);
    REGISTER_MSG_HANDLER(ConnectionActor, NamedMsgId::Get()->GetMsgId("core::net::on_packet"), OnMsg_Packet);
    REGISTER_MSG_HANDLER(ConnectionActor, NamedMsgId::Get()->GetMsgId("core::net::on_closed"), OnMsg_Closed);
  }
protected:
  virtual void OnConnected() {}
  virtual void OnClosed(int32_t reason) {}

  #define REGISTER_PACKET_HANDLER(ACTOR_CLASS, PACKET_CLASS, HANDLER_FN)\
  RegisterPacketHandler(PacketType(make_shared<PACKET_CLASS>()), boost::bind(&ACTOR_CLASS::HANDLER_FN, dynamic_pointer_cast<ACTOR_CLASS>(shared_from_this()), _1))
  void RegisterPacketHandler(const PacketType &packet_type, const PacketHandler &handler) {
    packet_handlers_[packet_type] = handler;
  }

  int32_t GetSessionId() {
    return session_id_;
  }
private:
  void OnMsg_Connected(int32_t sender_id, shared_ptr<ActorMsgParam> param) {
    auto mp = dynamic_pointer_cast<MP_I32>(param);
    if (!mp)
      return;
    session_id_ = mp->i;
    OnConnected();
  }
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
  void OnMsg_Closed(int32_t sender_id, shared_ptr<ActorMsgParam> param) {
    auto mp = dynamic_pointer_cast<MP_I32>(param);
    if (!mp)
      return;
    OnClosed(mp->i);
  }
private:
  int32_t session_id_;
  unordered_map<PacketType, PacketHandler> packet_handlers_;
};
