#pragma once
#include <memory>
using namespace std;
#include "../Util/Singleton.h"
#include "../Actor/ActorThread.h"
#include "../NetActor/TcpClientThread.h"
#include "../NetActor/ClientNetActor.h"

class ClientFrame {
  DECLARE_SINGLETON(ClientFrame)
public:
  // Only in main thread <begin>
  void Start(shared_ptr<ClientNetActor> client_net_actor) {
    actor_mgr_ = make_shared<ActorMgr>();
    actor_msg_q_ = make_shared<ActorMsgQ>();

    int32_t net_actor_id = actor_mgr_->AddActor(client_net_actor);
    tcp_client_thread_.Start(actor_msg_q_, net_actor_id);

    actor_thread_ = make_shared<ActorThread>(actor_mgr_, actor_msg_q_);
    actor_thread_->Start();
  }
  void Stop() {
    tcp_client_thread_.Stop();
    actor_thread_->Stop();

    tcp_client_thread_.join();
    actor_thread_->join();
  }
  // Only in main thread <end>

  void AddActor() {

  }
  void SendPacket(shared_ptr<::google::protobuf::Message> packet) {
    auto msg = make_shared<NetThreadMsg>(NetThreadMsg::SEND, 0, packet);
    tcp_client_thread_.PostMsg(msg);
  }
private:
  //ActorMgr actor_mgr;
  shared_ptr<ActorMgr> actor_mgr_;
  shared_ptr<ActorMsgQ> actor_msg_q_;
  shared_ptr<ActorThread> actor_thread_;
  TcpClientThread tcp_client_thread_;

  // ActorMsgQ


  
  // TcpClientThread

  // start
  // add actor
  // thread.start
};