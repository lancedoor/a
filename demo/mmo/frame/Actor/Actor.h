#pragma once
#include <boost/function.hpp>
#include "ActorMsgParam.h"

class Actor {
public:
	Actor() {

	}
	virtual ~Actor() {

	}
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
  void RegisterMsgHandler(int32_t msg_id, MsgHandler msg_handler){
    msg_handlers_[msg_id] = msg_handler;
  }
private:
  unordered_map<int32_t, MsgHandler> msg_handlers_;
protected:
	int32_t actor_id_;
};
