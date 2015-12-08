#pragma once
#include <mutex>
#include <vector>
#include "Singleton.h"
#include "Actor.h"

class ActorManager {
	_DECLARE_SINGLETON(ActorManager)
private:
	ActorManager() {
		actors_.push_back(nullptr);
		actors_.push_back(nullptr);
		actors_.push_back(make_shared<Actor>(3));
		actors_.push_back(make_shared<Actor>(4));
		actors_in_process_.resize(3);
	}
public:
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


	//int32_t AddActor(shared_ptr<Actor> actor);

	//shared_ptr<Actor> TakeActor(int32_t actor_id);
	//void PutActor(shared_ptr<Actor> actor);

private:
	mutex mutex_;
	vector<shared_ptr<Actor>> actors_;
	vector<bool> actors_in_process_;

};
