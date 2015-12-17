#pragma once
#include <memory>
using namespace std;
#include "../Util/Singleton.h"
#include "../NetActor/TcpClientThread.h"
#include "../NetActor/ClientNetActor.h"
#include "ActorFrame.h"

class ClientFrame : public ActorFrame {
  DECLARE_SINGLETON(ClientFrame)
public:
  // Can ONLY be invoked from main thread <begin>
  void Init(int32_t thread_count, shared_ptr<ClientNetActor> client_net_actor) {
    ActorFrame::Init(thread_count);
    client_net_actor_ = client_net_actor;
  }
  virtual void Start() {
    ActorFrame::Start();
    int32_t net_actor_id = actor_mgr_->AddActor(client_net_actor_);
    tcp_client_thread_.Start(actor_msg_q_, net_actor_id);
  }
  void Stop() {
    ActorFrame::Stop();
    tcp_client_thread_.Stop();
    tcp_client_thread_.join();
  }
  // Can ONLY be invoked from main thread <end>

  void SendPacket(shared_ptr<::google::protobuf::Message> packet) {
    auto msg = make_shared<NetThreadMsg>(NetThreadMsg::SEND, 0, packet);
    tcp_client_thread_.PostMsg(msg);
  }
private:
  shared_ptr<ClientNetActor> client_net_actor_;
  TcpClientThread tcp_client_thread_;
};