#pragma once
#include <memory>
#include <vector>
using namespace std;

template<typename THREAD_TYPE, int THREAD_COUNT>
class Threads {
public:
  Threads() {
    for (int i = 0; i < THREAD_COUNT; ++i) {
      auto t = make_shared<THREAD_TYPE>();
      threads_.push_back(t);
      t->Start();
    }
  }
  ~Threads() {
    for (auto t : threads_)
      t->Stop();
    for (auto t : threads_)
      t->join();
  }
private:
  vector<shared_ptr<THREAD_TYPE>> threads_;
};
