#pragma once
#include "ActorMsgQ.h"

struct MP_Packet : public ActorMsgQ::MsgParams {
  shared_ptr<::google::protobuf::Message> packet;
};
