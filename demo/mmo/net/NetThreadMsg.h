#pragma once
#include "google/protobuf/message.h"

struct NetThreadMsg {
  enum ECmdId {
    SEND,
    BROADCAST,
    CLOSE,

    _MAX
  };

  int32_t cmd_id;
  int32_t session_id;
  shared_ptr<::google::protobuf::Message> packet;

  shared_ptr<uint8_t> p_param;
  int32_t p_size;

  NetThreadMsg() {
    NetThreadMsg(ECmdId::_MAX, 0, nullptr);
  }
  NetThreadMsg(int32_t _cmd_id, int32_t _session_id, shared_ptr<::google::protobuf::Message> _packet) {
    cmd_id = _cmd_id;
    session_id = _session_id;
    packet = _packet;
  }
};
