#pragma once
#include <memory>
using namespace std;
#include "../Actor/ActorThread.h"

class ActorFrame {
public:
  ActorFrame() {
    thread_count_ = 1;
  }

  // Can ONLY be invoked from main thread <begin>
  void Init(int32_t thread_count) {
    thread_count_ = thread_count;
  }
  virtual void Start() {
    actor_mgr_ = make_shared<ActorMgr>();
    actor_msg_q_ = make_shared<ActorMsgQ>();
    for (int i = 0; i < thread_count_; ++i) {
      auto t = make_shared<ActorThread>(actor_mgr_, actor_msg_q_);
      actor_threads_.push_back(t);
      t->Start();
    }
  }
  virtual void Stop() {
    for (auto t : actor_threads_)
      t->Stop();
    for (auto t : actor_threads_)
      t->join();
  }
  // Can ONLY be invoked from main thread <end>

  template<typename ActorClass>
  int32_t AddActor() {
    if (!actor_mgr_)
      return -1;
    return actor_mgr_->AddActor(make_shared<ActorClass>());
  }
  void DeleteActor(int32_t actor_id) {
    if (!actor_mgr_)
      return;
    actor_mgr_->DeleteActor(actor_id);
  }

  void PostMsg(int32_t sender, int32_t receiver, int32_t msg_id, shared_ptr<ActorMsgParam> params) {
    actor_msg_q_->PostMsg(sender, receiver, msg_id, params);
  }

protected:
  int32_t thread_count_;

  shared_ptr<ActorMgr> actor_mgr_;
  shared_ptr<ActorMsgQ> actor_msg_q_;
  vector<shared_ptr<ActorThread>> actor_threads_;
};