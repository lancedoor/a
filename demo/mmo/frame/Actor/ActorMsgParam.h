#pragma once

struct ActorMsgParam {
  virtual ~ActorMsgParam() {}
};


struct MP_Void : public ActorMsgParam {
};
struct MP_I32 : public ActorMsgParam {
  int32_t i;
};
struct MP_I32_I32 : public ActorMsgParam {
  int32_t i;
  int32_t j;
};
