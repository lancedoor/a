#pragma once
#include <unordered_map>
using namespace std;
#include <boost/function.hpp>
#include "ActorMsgParam.h"

class Actor : public enable_shared_from_this<Actor> {
public:
	Actor() {}
	virtual ~Actor() {}
  virtual void Init() {}

  void SetActorId(int32_t actor_id) {
    actor_id_ = actor_id;
  }
  void OnMsg(int32_t sender_id, int32_t msg_id, shared_ptr<ActorMsgParam> params) {
    auto it = msg_handlers_.find(msg_id);
    if (it != msg_handlers_.end())
      it->second(sender_id, params);
  }
protected:
  typedef boost::function<void(int32_t sender_id, shared_ptr<ActorMsgParam>)> MsgHandler;
  #define REGISTER_MSG_HANDLER(ACTOR_CLASS, MSG_ID, HANDLER_FN)\
  RegisterMsgHandler(MSG_ID, boost::bind(&ACTOR_CLASS::HANDLER_FN, dynamic_pointer_cast<ACTOR_CLASS>(shared_from_this()), _1, _2))
  void RegisterMsgHandler(int32_t msg_id, MsgHandler msg_handler){
    msg_handlers_[msg_id] = msg_handler;
  }
  int32_t GetActorId() {
    return actor_id_;
  }
private:
  int32_t actor_id_;
  unordered_map<int32_t, MsgHandler> msg_handlers_;
};
