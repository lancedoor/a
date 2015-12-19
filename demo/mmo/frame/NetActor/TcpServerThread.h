#pragma once
#include <unordered_map>
#include "../Actor/ActorMsgQ.h"
#include "../Actor/ActorMgr.h"
#include "../Net/TcpServer.h"
#include "NetThread.h"
#include "NamedMsgId.h"
#include "MsgParamDefine.h"
#include "ConnectionActor.h"

typedef shared_ptr<ConnectionActor>(*ConnectionActorCreator)();

class TcpServerThread : public NetThread {
  class MyTcpServer : public TcpServer {
  public:
    MyTcpServer(shared_ptr<ActorMsgQ> actor_msg_q, 
                shared_ptr<ActorMgr> actor_mgr,
                ConnectionActorCreator actor_creator) {
      actor_msg_q_ = actor_msg_q;
      actor_mgr_ = actor_mgr;
      actor_creator_ = actor_creator;

      msg_id_on_connected_ = NamedMsgId::Get()->GetMsgId("core::net::on_connected");
      msg_id_on_packet_ = NamedMsgId::Get()->GetMsgId("core::net::on_packet");
      msg_id_on_closed_ = NamedMsgId::Get()->GetMsgId("core::net::on_closed");
    }
  private:
    virtual void OnNewSession(int32_t session_id) {
      auto actor_msg_q = actor_msg_q_.lock();
      if (!actor_msg_q)
        return;
      auto actor_mgr = actor_mgr_.lock();
      if (!actor_mgr)
        return;

      int32_t actor_id = actor_mgr->AddActor(actor_creator_());
      session_to_actor_[session_id] = actor_id;
      auto mp = make_shared<MP_I32>();
      mp->i = session_id;
      actor_msg_q->PostMsg(-1, actor_id, msg_id_on_connected_, mp);
    }
    virtual void OnSessionPacket(int32_t session_id, shared_ptr<::google::protobuf::Message> packet) {
      int32_t actor_id = GetActorId(session_id);
      if (actor_id == -1)
        return;
      auto actor_msg_q = actor_msg_q_.lock();
      if (!actor_msg_q)
        return;

      auto msg = make_shared<MP_Packet>();
      msg->packet = packet;
      actor_msg_q->PostMsg(-1, actor_id, msg_id_on_packet_, msg);
    }
    virtual void OnSessionClosed(int32_t session_id, int32_t reason) {
      int32_t actor_id = GetActorId(session_id);
      if (actor_id == -1)
        return;
      session_to_actor_.erase(session_id);

      auto actor_msg_q = actor_msg_q_.lock();
      if (!actor_msg_q)
        return;

      auto msg = make_shared<MP_I32>();
      msg->i = reason;
      actor_msg_q->PostMsg(-1, actor_id, msg_id_on_closed_, msg);
    }
  private:
    int32_t GetActorId(int32_t session_id) {
      auto it = session_to_actor_.find(session_id);
      return it == session_to_actor_.end() ? -1 : it->second;
    }
  private:
    weak_ptr<ActorMsgQ> actor_msg_q_;
    weak_ptr<ActorMgr> actor_mgr_;
    ConnectionActorCreator actor_creator_;
    int32_t msg_id_on_connected_;
    int32_t msg_id_on_packet_;
    int32_t msg_id_on_closed_;
    std::unordered_map<int32_t, int32_t> session_to_actor_;
  };
public:
  virtual void Start(shared_ptr<ActorMsgQ> actor_msg_q,
                    shared_ptr<ActorMgr> actor_mgr,
                    ConnectionActorCreator actor_creator) {
    tcp_server_ = make_shared<MyTcpServer>(actor_msg_q, actor_mgr, actor_creator);
    tcp_server_->Start();
    Thread::Start();
  }
  virtual void Stop() {
    tcp_server_->Stop();
    Thread::Stop();
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