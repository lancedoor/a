#pragma once
#include "Singleton.h"
#include "WorkerThread.h"

class WorkerThreadManager {
	DECLARE_SINGLETON(WorkerThreadManager)
public:
	void Start() {
		for (int i = 0; i < 2; ++i) {
			auto wt = make_shared<WorkerThread>();
			threads_.push_back(wt);
			wt->Start();
		}
	}
	void Stop() {
		for (auto wt : threads_)
			wt->Stop();
		for (auto wt : threads_)
			wt->join();
	}
private:
	vector<shared_ptr<WorkerThread>> threads_;
};