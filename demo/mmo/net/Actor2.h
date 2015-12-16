#pragma once
#include <boost/function.hpp>
#include "ActorMsgQ.h"

class Actor {
public:
	Actor() {

	}
	virtual ~Actor() {

	}
	void SetActorId(int32_t actor_id) {
		actor_id_ = actor_id;
	}
  void OnMsg(int32_t sender_id, int32_t msg_id, shared_ptr<ActorMsgQ::MsgParams> params) {
    auto it = msg_handlers_.find(msg_id);
    if (it != msg_handlers_.end())
      it->second(sender_id, params);
  }
protected:
  typedef boost::function<void(int32_t sender_id, shared_ptr<ActorMsgQ::MsgParams>)> MsgHandler;
  void RegisterMsgHandler(int32_t msg_id, MsgHandler msg_handler){
    msg_handlers_[msg_id] = msg_handler;
  }
private:
  unordered_map<int32_t, MsgHandler> msg_handlers_;
  //void PostMsg(int32_t receiver_id, int32_t msg_id, shared_ptr<ActorMsgParam> params) {
  //}
  //void PostMsgToFollowers(int32_t msg_id, shared_ptr<ActorMsgParam> params) {

  //}
  //void Follow(int32_t follower_id, int32_t msg_id) {

  //}

protected:
	int32_t actor_id_;

	//void PostMessage(int32_t actor_id, boost::function handler, boost::function callback);
	//void FollowEvent(int32_t actor_id, int32_t event_id, boost::function callback);

	//void Post();
	//void Do() {
	//	handler(this, )
	//}
	//void Handler();



	//ActorService s = {
	//	{SEND_TEXT, fn, params}    (string) -> void
	//}

	//bind(fn, Actor, )



	//	void Service(int32_t p1, const string &p2, const Object &p3) {

	//}

	//enum Service{

	//};


	// memo: event & service
	// q: param?

	//void Follow(int32_t actor_id, int32_t event_id);
	//void NotifyFollowers(int32_t local_event);

	//void Call(int32_t actor_id, int32_t service_id);
};
