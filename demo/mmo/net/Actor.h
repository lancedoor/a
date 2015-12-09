#pragma once
#include <boost/bind.hpp>
#include "MessageManager.h"

class Actor {
public:
	Actor() {

	}
	virtual ~Actor() {

	}
	void SetActorId(int32_t actor_id) {
		actor_id_ = actor_id;
	}

	void OnPacket(const string &packet) {
		cout << "Actor::OnPacket : " << packet << endl;
	}
	template<typename Fn, typename... Params>
	void PutMessage(int32_t receiver_id, Fn fn, Params... params) {
		MessageManager::Get()->PutMessage(actor_id_, receiver_id, fn, params...);
	}

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
