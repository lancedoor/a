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
      net_msg_id_on_packet_ = NamedMsgId::Get()->GetMsgId("core::net::on_packet");
    }
  private:
    virtual void OnConnected(const boost::system::error_code &ec) {}
    virtual void OnPacket(shared_ptr<::google::protobuf::Message> packet) {
      auto q = actor_msg_q_.lock();
      if (!q)
        return;

      auto msg = make_shared<MP_Packet>();
      msg->packet = packet;
      q->PostMsg(-1, net_actor_id_, net_msg_id_on_packet_, msg);
    }
    virtual void OnClosed(int32_t reason) {}
  private:
    weak_ptr<ActorMsgQ> actor_msg_q_;
    int32_t net_actor_id_;
    int32_t net_msg_id_on_packet_;
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