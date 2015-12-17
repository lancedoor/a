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

      msg_id_on_new_session_ = NamedMsgId::Get()->GetMsgId("core::net::on_new_session");
      msg_id_on_session_packet_ = NamedMsgId::Get()->GetMsgId("core::net::on_session_packet");
      msg_id_on_session_closed_ = NamedMsgId::Get()->GetMsgId("core::net::on_session_closed");
    }
  private:
    virtual void OnNewSession(int32_t session_id) {
      auto q = actor_msg_q_.lock();
      if (!q)
        return;

      auto msg = make_shared<MP_I32>();
      msg->i = session_id;
      q->PostMsg(-1, net_actor_id_, msg_id_on_new_session_, msg);
    }
    virtual void OnSessionPacket(int32_t session_id, shared_ptr<::google::protobuf::Message> packet) {
      auto q = actor_msg_q_.lock();
      if (!q)
        return;

      auto msg = make_shared<MP_SessionPacket>();
      msg->session_id = session_id;
      msg->packet = packet;
      q->PostMsg(-1, net_actor_id_, msg_id_on_session_packet_, msg);
    }
    virtual void OnSessionClosed(int32_t session_id, int32_t reason) {
      auto q = actor_msg_q_.lock();
      if (!q)
        return;

      auto msg = make_shared<MP_I32_I32>();
      msg->i = session_id;
      msg->j = reason;
      q->PostMsg(-1, net_actor_id_, msg_id_on_session_closed_, msg);
    }
  private:
    weak_ptr<ActorMsgQ> actor_msg_q_;
    int32_t net_actor_id_;
    int32_t msg_id_on_new_session_;
    int32_t msg_id_on_session_packet_;
    int32_t msg_id_on_session_closed_;
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