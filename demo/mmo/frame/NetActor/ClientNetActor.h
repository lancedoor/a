#pragma once
#include <boost/bind.hpp>
#include "../Net/PacketType.h"
#include "MsgParamDefine.h"
#include "NamedMsgId.h"
#include "ConnectionActor.h"

class ClientNetActor : public ConnectionActor {
  typedef boost::function<void(shared_ptr<::google::protobuf::Message>)> PacketHandler;
public:
  ClientNetActor() {
    RegisterMsgHandler(NamedMsgId::Get()->GetMsgId("core::net::on_connect_failed"), boost::bind(&ClientNetActor::OnMsg_ConnectFailed, this, _1, _2));
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
