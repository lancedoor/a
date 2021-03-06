#pragma once
#include <boost/bind.hpp>
#include "../Net/PacketType.h"
#include "MsgParamDefine.h"
#include "NamedMsgId.h"
#include "ConnectionActor.h"

class ClientNetActor : public ConnectionActor {
  typedef boost::function<void(shared_ptr<::google::protobuf::Message>)> PacketHandler;
public:
  virtual void Init() {
    ConnectionActor::Init();
    REGISTER_MSG_HANDLER(ClientNetActor, NamedMsgId::Get()->GetMsgId("core::net::on_connect_failed"), OnMsg_ConnectFailed);
  }
protected:
  virtual void OnConnectFailed(int32_t error_code) {}
private:
  void OnMsg_ConnectFailed(int32_t sender_id, shared_ptr<ActorMsgParam> param) {
    auto  mp = dynamic_pointer_cast<MP_I32>(param);
    if (!mp)
      return;
    OnConnectFailed(mp->i);
  }
};
