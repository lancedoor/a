#pragma once
#include "Thread.h"
#include "MessageManager.h"
#include "TcpAcceptor.h"
#include "CmdQueue.h"
using namespace std;

class TcpServer : public Thread {
public:
	TcpServer() {
		stop_ = false;

		sessions_.resize(1);
	}
	~TcpServer() {
	}
	void Stop() {
		Thread::Stop();
		io_service_.stop();
		join();
	}
	void PutCmd(unique_ptr<string> cmd) {
		cmd_queue_.PutCmd(move(cmd));
	}

	void SetHandler_OnConnected(int32_t session_id);
	void SetHandler_OnPacket(int32_t session_id, void *packet, int32_t size);
	void SendTo(int32_t session_id, void *packet, int32_t size);

private:
	virtual void ThreadProc() {
		TcpAcceptor acceptor(io_service_);
		acceptor.SetHandler_OnAccepted(boost::bind(&TcpServer::OnAccepted, this, _1));
		acceptor.Start();

		while (!stop_) {
			io_service_.poll();

			auto cmd = cmd_queue_.GetCmd();
			if (cmd) {
				auto session = GetSession(0);
				if (session)
					session->Send(*cmd);
			}
			
			//auto q = MessageManager::Get()->GetMessageQueue(1);
			//if (q) {
			//	for (const auto &msg : *q) {
			//		if (msg) {
			//			auto session = GetSession(0);
			//			if (session)
			//				session->Send(msg->message_params);
			//		}
			//	}
			//}

			this_thread::sleep_for(chrono::milliseconds(1));
		}
		cout << "TcpServer:ThreadProc End" << endl;
	}


	int32_t GetUnusedSessionId() {
		return 0;
	}
	shared_ptr<TcpConnection> GetSession(int32_t session_id) {
		if (session_id < 0 || session_id >= (int32_t)sessions_.size())
			return nullptr;
		else
			return sessions_[session_id];
	}
	void OnAccepted(shared_ptr<TcpConnection> connection) {
		int32_t session_id = GetUnusedSessionId();
		if (session_id == -1) {
			//todo: send prompting message
			connection->Close();
		} else {
			sessions_[session_id] = connection;
			connection->SetHandler_OnPacket(boost::bind(&TcpServer::OnPacket, this, _1));
			connection->Start();
		}
	}
	void OnPacket(const string &s) {
		cout << s << endl;
		MessageManager::Get()->PutMessage(1, 2, &Actor::OnPacket, s);
	}

private:
	boost::asio::io_service io_service_;

	vector<shared_ptr<TcpConnection>> sessions_;
	CmdQueue cmd_queue_;
};