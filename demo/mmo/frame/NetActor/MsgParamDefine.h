#pragma once
#include <google/protobuf/message.h>
#include "../Actor/ActorMsgParam.h"

struct MP_Packet : public ActorMsgParam {
  shared_ptr<::google::protobuf::Message> packet;
};
struct MP_SessionPacket : public ActorMsgParam {
  int32_t session_id;
  shared_ptr<::google::protobuf::Message> packet;
};

struct MP_Net : public ActorMsgParam {
  enum EMsgType {
    CONNECT,
    SEND,
    ON_PACKET,
    ON_CLOSE,
  };
  int32_t msg_type;
  shared_ptr<::google::protobuf::Message> packet;
};
