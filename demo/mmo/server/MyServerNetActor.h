#pragma once
#include "../frame/NetActor/ServerNetActor.h"
#include "../frame/Frame/ServerFrame.h"
#include "Broker.h"

class MyServerNetActor : public ServerNetActor {
public:
  MyServerNetActor() {
    RegisterPacketHandler(PacketType(make_shared<Packet::CS_Say>()), boost::bind(&MyServerNetActor::OnPacket_CS_Say, this, _1, _2));
  }
private:
  virtual void OnNewSession(int32_t session_id) {
    cout << "MyServerNetActor::OnNewSession(" + to_string(session_id) + ")\n";
    int32_t broker_id = ServerFrame::Get()->AddActor<Broker>();
    session_to_broker_[session_id] = broker_id;
    auto mp = make_shared<MP_I32>();
    mp->i = session_id;
    ServerFrame::Get()->PostMsg(GetActorId(), broker_id, NamedMsgId::Get()->GetMsgId("core::net::on_connected"), mp);
  }
  virtual void OnSessionClosed(int32_t session_id, int32_t reason) {}

  void OnPacket_CS_Say(int32_t session_id, shared_ptr<::google::protobuf::Message> _packet) {
    auto packet = dynamic_pointer_cast<Packet::CS_Say>(_packet);
    if (!packet)
      return;
    cout << to_string(session_id) + ": " + packet->text() + "\n";
  }
private:
  unordered_map<int32_t, int32_t> session_to_broker_;
};
