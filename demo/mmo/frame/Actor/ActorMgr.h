#pragma once
#include <mutex>
#include "../Util/IdPtrMap.h"
#include "Actor.h"

class ActorMgr {
public:
  ActorMgr() {
  }

  int32_t AddActor(shared_ptr<Actor> actor) {
    lock_guard<mutex> lg(mutex_);
    auto actor_item = make_shared<ActorItem>();
    actor_item->status = kSuspending;
    actor_item->actor = actor;
    int32_t actor_id = actor_items_.AddItem(actor_item);
    actor->SetActorId(actor_id);
    return actor_id;
  }
  void DeleteActor(int32_t actor_id) {
    //todo: 当Actor被删后，往原ActorId发送的消息如何不被新的持有同ActorId的对象接收。（目前使用Id递增避免了部分问题）
    lock_guard<mutex> lg(mutex_);
    auto actor_item = actor_items_.GetItem(actor_id);
    if (!actor_item)
      return;

    switch (actor_item->status) {
    case kSuspending: {
      actor_items_.erase(actor_id);
    } break;
    case kProcessing: {
      actor_item->status = kMarkDelete;
    } break;
    case kMarkDelete: break;
    default:assert(false); break; //handle this if there is a new status
    }
  }

	shared_ptr<Actor> StartActorProcess(int32_t actor_id) {
		lock_guard<mutex> lg(mutex_);
    auto actor_item = actor_items_.GetItem(actor_id);
    if (!actor_item)
      return nullptr;

    switch (actor_item->status) {
    case kSuspending: {
      actor_item->status = kProcessing;
      return actor_item->actor;
    } break;
    case kProcessing: return nullptr;
    case kMarkDelete: return nullptr;
    default:assert(false); return nullptr; //handle this if there is a new status
    }
	}

	void EndActorProcess(int32_t actor_id) {
		lock_guard<mutex> lg(mutex_);
    auto actor_item = actor_items_.GetItem(actor_id);
    if (!actor_item)
      return;

    switch (actor_item->status) {
    case kSuspending: assert(false); break; // How could this happen?
    case kProcessing: {
      actor_item->status = kSuspending;
    } break;
    case kMarkDelete: {
      actor_items_.erase(actor_id);
    } break;
    default:assert(false); return; //handle this if there is a new status
    }
  }
private:
	mutex mutex_;
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
  IdPtrMap<ActorItem> actor_items_;
};
