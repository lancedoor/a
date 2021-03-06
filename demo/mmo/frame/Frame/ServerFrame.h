#pragma once
#include "../Util/Singleton.h"
#include "../NetActor/TcpServerThread.h"
#include "ActorFrame.h"

class ServerFrame : public ActorFrame {
  DECLARE_SINGLETON(ServerFrame)
public:
  // Can ONLY be invoked from main thread <begin>
  void Init(int32_t thread_count, ConnectionActorCreator actor_creator) {
    ActorFrame::Init(thread_count);
    actor_creator_ = actor_creator;
  }
  virtual void Start() {
    ActorFrame::Start();
    tcp_server_thread_.Start(actor_msg_q_, actor_mgr_, actor_creator_);
  }
  virtual void Stop() {
    ActorFrame::Stop();
    tcp_server_thread_.Stop();
    tcp_server_thread_.join();
  }
  // Can ONLY be invoked from main thread <end>

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
  ConnectionActorCreator actor_creator_;
};