#pragma once
#include "../Util/Singleton.h"
#include "../NetActor/TcpServerThread.h"
#include "../NetActor/ServerNetActor.h"
#include "ActorFrame.h"

class ServerFrame : public ActorFrame {
  DECLARE_SINGLETON(ServerFrame)
public:
  // Only in main thread <begin>
  void Init(shared_ptr<ServerNetActor> server_net_actor) {
    server_net_actor_ = server_net_actor;
  }
  virtual void Start() {
    ActorFrame::Start();
    int32_t net_actor_id = actor_mgr_->AddActor(server_net_actor_);
    tcp_server_thread_.Start(actor_msg_q_, net_actor_id);
  }
  virtual void Stop() {
    ActorFrame::Stop();
    tcp_server_thread_.Stop();
  }
  // Only in main thread <end>

  void SendPacket(int32_t session_id, shared_ptr<::google::protobuf::Message> packet) {
    auto msg = make_shared<NetThreadMsg>(NetThreadMsg::SEND, session_id, packet);
    tcp_server_thread_.PostMsg(msg);
  }
  void BroadcastPacket(shared_ptr<::google::protobuf::Message> packet) {
    auto msg = make_shared<NetThreadMsg>(NetThreadMsg::BROADCAST, -1, packet);
    tcp_server_thread_.PostMsg(msg);
  }
private:
  TcpServerThread tcp_server_thread_;
  shared_ptr<ServerNetActor> server_net_actor_;
};