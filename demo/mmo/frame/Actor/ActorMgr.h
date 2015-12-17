#pragma once
#include <mutex>
#include <vector>
#include "Actor.h"

class ActorMgr {
public:
	int32_t AddActor(shared_ptr<Actor> actor) {
		lock_guard<mutex> lg(mutex_);
		int32_t actor_id = actors_.size();
		actor->SetActorId(actor_id);
		actors_.push_back(actor);
		actors_in_process_.push_back(false);
		return actor_id;
	}

	shared_ptr<Actor> StartActorProcess(int32_t actor_id) {
		lock_guard<mutex> lg(mutex_);
		if (actor_id < 0 || actor_id >= (int32_t) actors_.size())
			return nullptr;

		if (actors_in_process_[actor_id])
			return nullptr;

		actors_in_process_[actor_id] = true;
		return actors_[actor_id];
	}

	void EndActorProcess(int32_t actor_id) {
		lock_guard<mutex> lg(mutex_);
		if (actor_id < 0 || actor_id >= (int32_t) actors_.size())
			return;
		actors_in_process_[actor_id] = false;
	}
private:
	mutex mutex_;
	vector<shared_ptr<Actor>> actors_;
	vector<bool> actors_in_process_;

};
