#pragma once
#include "Thread.h"
#include "ActorMsgQ.h"
#include "ActorMgr.h"

class ActorThread : public Thread {
	virtual void ThreadProc() {
		while (!stop_) {
			this_thread::sleep_for(chrono::milliseconds(1));

			auto q = ActorMsgQ::Get()->StartMessageQueueProcess();
			if (!q)
				continue;
			int32_t receiver_id = (*q->begin())->receiver;
			auto actor = ActorMgr::Get()->StartActorProcess(receiver_id);
			if (actor) {
				for (auto msg : *q) {
          actor->OnMsg(msg->sender, msg->msg_id, msg->params);
				}
			}
      ActorMgr::Get()->EndActorProcess(receiver_id);
      ActorMsgQ::Get()->EndMessageQueueProcess(receiver_id);
		}
	}
};
