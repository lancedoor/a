#pragma once
#include <mutex>
//#include <vector>
#include <unordered_map>
#include "Actor.h"

class ActorMgr {
public:
  ActorMgr() {
    next_actor_id_ = 0;
  }

  int32_t AddActor(shared_ptr<Actor> actor) {
    lock_guard<mutex> lg(mutex_);

    //int32_t actor_id = actors_.size();
    //actor->SetActorId(actor_id);
    //actors_.push_back(actor);
    //actors_in_process_.push_back(false);

    int32_t actor_id = GetNextActorId();
    actor->SetActorId(actor_id);
    auto actor_item = make_shared<ActorItem>();
    actor_item->status = kSuspending;
    actor_item->actor = actor;
    actor_items_[actor_id] = actor_item;

    return actor_id;
  }
  void DeleteActor(int32_t actor_id) {
    //todo: 当Actor被删后，往原ActorId发送的消息如何不被新的持有同ActorId的对象接收。（目前使用Id递增避免了部分问题）
    lock_guard<mutex> lg(mutex_);
    auto it = actor_items_.find(actor_id);
    if (it == actor_items_.end())
      return;

    switch (it->second->status) {
    case kSuspending: {
      actor_items_.erase(it);
    } break;
    case kProcessing: {
      it->second->status = kMarkDelete;
    } break;
    case kMarkDelete: break;
    default:assert(false); break; //handle this if there is a new status
    }
  }

	shared_ptr<Actor> StartActorProcess(int32_t actor_id) {
		lock_guard<mutex> lg(mutex_);

		//if (actor_id < 0 || actor_id >= (int32_t) actors_.size())
		//	return nullptr;

		//if (actors_in_process_[actor_id])
		//	return nullptr;

		//actors_in_process_[actor_id] = true;
		//return actors_[actor_id];

    auto it = actor_items_.find(actor_id);
    if (it == actor_items_.end())
      return nullptr;

    switch (it->second->status) {
    case kSuspending: {
      it->second->status = kProcessing;
      return it->second->actor;
    } break;
    case kProcessing: return nullptr;
    case kMarkDelete: return nullptr;
    default:assert(false); return nullptr; //handle this if there is a new status
    }
	}

	void EndActorProcess(int32_t actor_id) {
		lock_guard<mutex> lg(mutex_);
		
    //if (actor_id < 0 || actor_id >= (int32_t) actors_.size())
		//	return;
		//actors_in_process_[actor_id] = false;

    auto it = actor_items_.find(actor_id);
    if (it == actor_items_.end())
      return;

    switch (it->second->status) {
    case kSuspending: assert(false); break; // How could this happen?
    case kProcessing: {
      it->second->status = kSuspending;
    } break;
    case kMarkDelete: {
      actor_items_.erase(it);
    } break;
    default:assert(false); return; //handle this if there is a new status
    }
  }
private:
  int32_t GetNextActorId() {
    while (actor_items_.find(next_actor_id_) != actor_items_.end()) {
      ++next_actor_id_;
    }
    int32_t ret = next_actor_id_;
    next_actor_id_ = next_actor_id_ == INT32_MAX ? 0 : next_actor_id_ + 1;
    return ret;
  }
private:
	mutex mutex_;
	//vector<shared_ptr<Actor>> actors_;
	//vector<bool> actors_in_process_;

  // ActorStatus:
  // (NotExist) <-> Suspending <-> Processing -> MarkDelete
  //    /|\                                         |
  //     |__________________________________________|
  enum EActorStatus {
    kSuspending,
    kProcessing,
    kMarkDelete
  };
  struct ActorItem {
    shared_ptr<Actor> actor;
    EActorStatus status;
  };
  unordered_map<int32_t, shared_ptr<ActorItem>> actor_items_;
  int32_t next_actor_id_;
};
