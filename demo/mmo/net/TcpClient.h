#pragma once
#include "Thread.h"
#include "MessageManager.h"
#include "CmdQueue.h"
using namespace std;

class TcpClient : public Thread {
public:
	TcpClient() {
		stop_ = false;
	}
	~TcpClient() {
	}
	void Stop() {
		io_service_.stop();
		stop_ = true;
		join();
	}
	void PutCmd(unique_ptr<string> cmd) {
		cmd_queue_.PutCmd(move(cmd));
	}

private:
	virtual void ThreadProc() {
		auto conn = TcpConnector::Connect(io_service_);
		if (!conn)
			return;

		conn->SetHandler_OnPacket(boost::bind(&TcpClient::OnPacket, this, _1));
		conn->SetHandler_OnClose(boost::bind(&TcpClient::OnClose, this, _1));
		conn->Start();
		while(!stop_) {
			io_service_.poll();

			auto cmd = cmd_queue_.GetCmd();
			if (cmd) {
				conn->Send(*cmd);
			}

			//auto q = MessageManager::Get()->GetMessageQueue(1);
			//if (q) {
			//	for (const auto &msg : *q) {
			//		if (msg)
			//			conn->Send(msg->message_params);
			//	}
			//}
			this_thread::sleep_for(chrono::milliseconds(1));
		}
		conn->Close();
		cout << "TcpClient:ThreadProc End" << endl;
	}
	void OnPacket(const string &s) {
		cout << s << endl;
	}
	void OnClose(int32_t reason) {
		cout << "TcpClient::OnClose(" << reason << ")" << endl;
	}

private:
	boost::asio::io_service io_service_;
	thread thread_;
	bool stop_;
	CmdQueue cmd_queue_;
};