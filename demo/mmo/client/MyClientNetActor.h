#pragma once
#include "../frame/NetActor/ClientNetActor.h"

class MyClientNetActor : public ClientNetActor {
public:
  virtual void Init() {
    ClientNetActor::Init();
    REGISTER_PACKET_HANDLER(MyClientNetActor, Packet::SC_LoginResult, OnPacket_SC_LoginResult);
    REGISTER_PACKET_HANDLER(MyClientNetActor, Packet::SC_SomeoneSay, OnPacket_SC_SomeoneSay);
  }
protected:
  virtual void OnConnectFailed(int32_t error_code) {
    cout << "MyClientNetActor::OnConnectFailed\n";
  }
  virtual void OnConnected() {
    cout << "MyClientNetActor::OnConnected\n";
    auto packet = make_shared<Packet::CS_Login>();
    packet->set_user("guest");
    ClientFrame::Get()->SendPacket(packet);
  }
  virtual void OnClosed(int32_t reason) {
    cout << "MyClientNetActor::OnClosed(" + to_string(reason) + ")\n";
  }
private:
  void OnPacket_SC_LoginResult(shared_ptr<::google::protobuf::Message> _packet) {
    auto packet = dynamic_pointer_cast<Packet::SC_LoginResult>(_packet);
    if (!packet)
      return;
    cout << packet->name() + ", " + packet->welcome() + "\n";
  }
  void OnPacket_SC_SomeoneSay(shared_ptr<::google::protobuf::Message> _packet) {
    auto packet = dynamic_pointer_cast<Packet::SC_SomeoneSay>(_packet);
    if (!packet)
      return;
    cout << packet->name() + ": " + packet->text() + "\n";
  }
};
