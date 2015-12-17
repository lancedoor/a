#pragma once
#include "../Util/Thread.h"
#include "ActorMsgQ.h"
#include "ActorMgr.h"

class ActorThread : public Thread {
public:
  ActorThread(shared_ptr<ActorMgr> actor_mgr, shared_ptr<ActorMsgQ> actor_msg_q) {
    actor_mgr_ = actor_mgr;
    actor_msg_q_ = actor_msg_q;
  }
private:
	virtual void ThreadProc() {
		while (!stop_) {
			this_thread::sleep_for(chrono::milliseconds(1));

      auto actor_mgr = actor_mgr_.lock();
      if (!actor_mgr)
        continue;
      auto actor_msg_q = actor_msg_q_.lock();
      if (!actor_msg_q)
        continue;

			auto q = actor_msg_q->StartMessageQueueProcess();
			if (!q)
				continue;
			int32_t receiver_id = (*q->begin())->receiver;
			auto actor = actor_mgr->StartActorProcess(receiver_id);
			if (actor) {
				for (auto msg : *q) {
          actor->OnMsg(msg->sender, msg->msg_id, msg->params);
				}
			}
      actor_mgr->EndActorProcess(receiver_id);
      actor_msg_q->EndMessageQueueProcess(receiver_id);
		}
	}
private:
  weak_ptr<ActorMgr> actor_mgr_;
  weak_ptr<ActorMsgQ> actor_msg_q_;
};
