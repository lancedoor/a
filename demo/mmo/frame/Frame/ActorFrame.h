#pragma once
#include <memory>
using namespace std;
#include "../Actor/ActorThread.h"

class ActorFrame {
public:
  // Can ONLY be invoked from main thread <begin>
  virtual void Start() {
    actor_mgr_ = make_shared<ActorMgr>();
    actor_msg_q_ = make_shared<ActorMsgQ>();
    actor_thread_ = make_shared<ActorThread>(actor_mgr_, actor_msg_q_);
    actor_thread_->Start();
  }
  virtual void Stop() {
    actor_thread_->Stop();
    actor_thread_->join();
  }
  // Can ONLY be invoked from main thread <end>

  void AddActor() {
  }

protected:
  shared_ptr<ActorMgr> actor_mgr_;
  shared_ptr<ActorMsgQ> actor_msg_q_;
  shared_ptr<ActorThread> actor_thread_;
};