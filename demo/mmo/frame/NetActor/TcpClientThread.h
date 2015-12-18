#pragma once
#include "../Actor/ActorMsgQ.h"
#include "../Net/TcpClient.h"
#include "NetThread.h"
#include "NamedMsgId.h"
#include "MsgParamDefine.h"

class TcpClientThread : public NetThread {
  class MyTcpClient : public TcpClient {
  public:
    MyTcpClient(shared_ptr<ActorMsgQ> actor_msg_q, int32_t net_actor_id) {
      actor_msg_q_ = actor_msg_q;
      net_actor_id_ = net_actor_id;

      msg_id_on_connect_failed_ = NamedMsgId::Get()->GetMsgId("core::net::on_connect_failed");
      msg_id_on_connected_ = NamedMsgId::Get()->GetMsgId("core::net::on_connected");
      msg_id_on_packet_ = NamedMsgId::Get()->GetMsgId("core::net::on_packet");
      msg_id_on_closed_ = NamedMsgId::Get()->GetMsgId("core::net::on_closed");
    }
  private:
    virtual void OnConnected(const boost::system::error_code &ec) {
      auto q = actor_msg_q_.lock();
      if (!q)
        return;

      auto mp = make_shared<MP_I32>();

      if (ec) {
        mp->i = ec.value();
        q->PostMsg(-1, net_actor_id_, msg_id_on_connect_failed_, mp);
      } else {
        mp->i = 0;
        q->PostMsg(-1, net_actor_id_, msg_id_on_connected_, mp);
      }
    }
    virtual void OnPacket(shared_ptr<::google::protobuf::Message> packet) {
      auto q = actor_msg_q_.lock();
      if (!q)
        return;

      auto mp = make_shared<MP_Packet>();
      mp->packet = packet;
      q->PostMsg(-1, net_actor_id_, msg_id_on_packet_, mp);
    }
    virtual void OnClosed(int32_t reason) {
      auto q = actor_msg_q_.lock();
      if (!q)
        return;

      auto mp = make_shared<MP_I32>();
      mp->i = reason;
      q->PostMsg(-1, net_actor_id_, msg_id_on_closed_, mp);
    }
  private:
    weak_ptr<ActorMsgQ> actor_msg_q_;
    int32_t net_actor_id_;
    int32_t msg_id_on_connect_failed_;
    int32_t msg_id_on_connected_;
    int32_t msg_id_on_packet_;
    int32_t msg_id_on_closed_;
  };
public:
  virtual void Start(shared_ptr<ActorMsgQ> actor_msg_q, int32_t net_actor_id) {
    tcp_client_ = make_shared<MyTcpClient>(actor_msg_q, net_actor_id);
    tcp_client_->Start();
    Thread::Start();
  }
  virtual void Stop() {
    tcp_client_->Stop();
    Thread::Stop();
    join();
    tcp_client_ = nullptr;
  }

private:
  virtual void ThreadProc() {
    while (!stop_) {
      tcp_client_->Poll();

      auto msg = msg_q_.PopFront();
      if (msg) {
        switch (msg->cmd_id)
        {
        case NetThreadMsg::ECmdId::SEND:
          tcp_client_->SendPacket(msg->packet);
        default:
          break;
        }
      }

      this_thread::sleep_for(chrono::milliseconds(1));
    }
  }
private:
  shared_ptr<MyTcpClient> tcp_client_;
};