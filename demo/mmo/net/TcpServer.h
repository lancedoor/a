#pragma once
#include "Thread.h"
#include "MessageManager.h"
#include "TcpAcceptor.h"
#include "CmdQueue.h"
#include "Sessions.h"
using namespace std;

class TcpServer : public Thread {
public:
	TcpServer()
	: sessions_(2) {
		stop_ = false;
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

private:
	virtual void ThreadProc() {

		TcpAcceptor acceptor(io_service_);
		acceptor.SetHandler_OnAccepted(boost::bind(&TcpServer::OnAccepted, this, _1));
		acceptor.Start();

		while (!stop_) {
			io_service_.poll();

			auto cmd = cmd_queue_.GetCmd();
			if (cmd) {
				auto session = sessions_.GetSession(0);
				if (session)
					session->Send(*cmd);
			}
			this_thread::sleep_for(chrono::milliseconds(1));
		}
		sessions_.CloseAllSessions();
		cout << "TcpServer:ThreadProc End" << endl;
	}

	void OnAccepted(shared_ptr<TcpConnection> connection) {
		int32_t session_id = sessions_.GetUnusedSessionId();
		if (session_id == -1) {
			//todo: send prompting message
			connection->Close();
		} else {
			connection->SetHandler_OnPacket(boost::bind(&TcpServer::OnPacket, this, session_id, _1));
			connection->SetHandler_OnClose(boost::bind(&TcpServer::OnClose, this, session_id, _1));
			connection->Start();
			sessions_.SetSession(session_id, connection);
		}
	}
	void OnPacket(int32_t session_id, const string &s) {
		cout << s << endl;
		MessageManager::Get()->PutMessage(1, 2, &Actor::OnPacket, s);
	}
	void OnClose(int32_t session_id, int32_t reason) {
		cout << "TcpServer::OnClose(" << session_id << ", " << reason << ")" << endl;
		sessions_.ReleaseSession(session_id);
	}

private:
	boost::asio::io_service io_service_;

	CmdQueue cmd_queue_;
	Sessions sessions_;
};