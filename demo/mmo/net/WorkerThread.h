#pragma once
#include "Thread.h"
#include "MessageManager.h"
#include "ActorManager.h"

class WorkerThread : public Thread {
	virtual void ThreadProc() {
		while (!stop_) {
			this_thread::sleep_for(chrono::milliseconds(1));

			auto q = MessageManager::Get()->StartMessageQueueProcess();
			if (!q)
				continue;
			int32_t receiver_id = (*q->begin())->receiver;
			auto actor = ActorManager::Get()->StartActorProcess(receiver_id);
			if (actor) {
				for (auto msg : *q) {
					if (msg->fn)
						msg->fn(actor);
				}
			}
			ActorManager::Get()->EndActorProcess(receiver_id);
			MessageManager::Get()->EndMessageQueueProcess(receiver_id);
		}
	}
};
