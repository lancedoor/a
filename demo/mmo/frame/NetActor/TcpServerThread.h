#pragma once
#include "../Actor/ActorMsgQ.h"
#include "../Net/TcpServer.h"
#include "NetThread.h"
#include "NamedMsgId.h"
#include "MsgParamDefine.h"

class TcpServerThread : public NetThread {
  class MyTcpServer : public TcpServer {
  public:
    MyTcpServer(shared_ptr<ActorMsgQ> actor_msg_q, int32_t net_actor_id) {
      actor_msg_q_ = actor_msg_q;
      net_actor_id_ = net_actor_id;
      net_msg_id_on_packet_ = NamedMsgId::Get()->GetMsgId("core::net::on_session_packet");
    }
  private:
    virtual void OnNewSession(int32_t session_id) {}
    virtual void OnSessionPacket(int32_t session_id, shared_ptr<::google::protobuf::Message> packet) {
      auto q = actor_msg_q_.lock();
      if (!q)
        return;

      auto msg = make_shared<MP_SessionPacket>();
      msg->session_id = session_id;
      msg->packet = packet;
      q->PostMsg(-1, net_actor_id_, net_msg_id_on_packet_, msg);
    }
    virtual void OnSessionClosed(int32_t session_id, int32_t reason) {}
  private:
    weak_ptr<ActorMsgQ> actor_msg_q_;
    int32_t net_actor_id_;
    int32_t net_msg_id_on_packet_;
  };
public:
  virtual void Start(shared_ptr<ActorMsgQ> actor_msg_q, int32_t net_actor_id) {
    tcp_server_ = make_shared<MyTcpServer>(actor_msg_q, net_actor_id);
    tcp_server_->Start();
    Thread::Start();
  }
  virtual void Stop() {
    tcp_server_->Stop();
    Thread::Stop();
    join();
    tcp_server_ = nullptr;
  }

private:
  virtual void ThreadProc() {
    while (!stop_) {
      tcp_server_->Poll();

      auto msg = msg_q_.PopFront();
      if (msg) {
        switch (msg->cmd_id)
        {
        case NetThreadMsg::ECmdId::SEND:
          tcp_server_->SendPacket(msg->session_id, msg->packet);
          break;
        case NetThreadMsg::ECmdId::BROADCAST:
          tcp_server_->BroadcastPacket(msg->packet);
          break;
        default:
          break;
        }
      }

      this_thread::sleep_for(chrono::milliseconds(1));
    }
  }
private:
  shared_ptr<MyTcpServer> tcp_server_;
};