#pragma once
#include "../frame/NetActor/ConnectionActor.h"
#include "../frame/Net/PacketType.h"
#include "../common/Packets.pb.h"

class Broker : public ConnectionActor {
public:
  virtual void Init() {
    ConnectionActor::Init();
    REGISTER_PACKET_HANDLER(Broker, Packet::CS_Login, OnPacket_CS_Login);
    REGISTER_PACKET_HANDLER(Broker, Packet::CS_Say, OnPacket_CS_Say);
  }
protected:
  virtual void OnConnected() {
    cout << "Broker::OnConnected(" + to_string(GetActorId()) + "," + to_string(GetSessionId()) + ")\n";
  }
  virtual void OnClosed(int32_t reason) {
    cout << "Broker::OnClosed(" + to_string(GetActorId()) + "," + to_string(GetSessionId()) + "," + to_string(reason) + ")\n";
    ServerFrame::Get()->DeleteActor(GetActorId());
  }

  void OnPacket_CS_Login(shared_ptr<::google::protobuf::Message> _packet) {
    auto packet = dynamic_pointer_cast<Packet::CS_Login>(_packet);
    if (!packet)
      return;
    cout << "Broker::OnPacket_CS_Login(user=" << packet->user() << ")" << endl;

    name_ = "guest" + to_string(GetActorId());
    auto reply_packet = make_shared<Packet::SC_LoginResult>();
    reply_packet->set_name(name_);
    reply_packet->set_welcome("welcome");
    SendToClient(reply_packet);
  }

  void OnPacket_CS_Say(shared_ptr<::google::protobuf::Message> _packet) {
    auto packet = dynamic_pointer_cast<Packet::CS_Say>(_packet);
    if (!packet)
      return;

    cout << name_ + ":" + packet->text() << endl;

    auto reply_packet = make_shared<Packet::SC_SomeoneSay>();
    reply_packet->set_name(name_);
    reply_packet->set_text(packet->text());

    Broadcast(reply_packet);
  }
private:
  void SendToClient(shared_ptr<::google::protobuf::Message> packet) {
    ServerFrame::Get()->SendPacket(GetSessionId(), packet);
  }
  void Broadcast(shared_ptr<::google::protobuf::Message> packet) {
    ServerFrame::Get()->BroadcastPacket(packet);
  }
private:
  string name_;
};