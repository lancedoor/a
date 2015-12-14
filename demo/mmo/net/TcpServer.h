#pragma once
#include <boost/asio.hpp>
#include "Thread.h"
#include "MessageManager.h"
#include "TcpAcceptor.h"
#include "CmdQueue.h"
#include "Sessions.h"
using namespace std;

#include "ServerConnection.h"

class TcpServer : public Thread, public enable_shared_from_this<TcpServer> {
  enum ECmdId {
    SEND,
    BROADCAST
  };

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
  void SendPacket(int32_t session_id, shared_ptr<uint8_t> &ptr, int32_t size) {
    PutCmd(make_shared<Cmd>(ECmdId::SEND, session_id, ptr, size));
  }
  void BroadcastPacket(shared_ptr<uint8_t> &ptr, int32_t size) {
    PutCmd(make_shared<Cmd>(ECmdId::BROADCAST, 0, ptr, size));
  }
  void SendPacket(int32_t session_id, const string &s) { //temp
    auto p = new uint8_t[s.size()];
    memcpy(p, s.data(), s.size());
    SendPacket(session_id, shared_ptr<uint8_t>(p), s.size());
  }
  void BroadcastPacket(const string &s) { //temp
    auto p = new uint8_t[s.size()];
    memcpy(p, s.data(), s.size());
    BroadcastPacket(shared_ptr<uint8_t>(p), s.size());
  }
public:
  virtual void OnSessionPacket(int32_t session_id, shared_ptr<::google::protobuf::Message> packet) {}
private:
	virtual void OnNewSession(int32_t session_id) {}
	//virtual void OnSessionPacket(int32_t session_id, uint8_t *ptr, uint32_t size) {}
	virtual void OnSessionClosed(int32_t session_id, int32_t reason) {}
private:
	virtual void ThreadProc() {

		TcpAcceptor acceptor(io_service_);
		acceptor.SetHandler_OnAccepted(boost::bind(&TcpServer::OnAccepted, this, _1));
		acceptor.Start();

		while (!stop_) {
			io_service_.poll();

      auto cmd = GetCmd();
			if (cmd) {
        switch (cmd->id) {
        case ECmdId::SEND: {
          auto session = sessions_.GetSession(cmd->i_param);
          if (session)
            session->Send(cmd->p_param, cmd->p_size);
          break;
        }
        case ECmdId::BROADCAST: {
          sessions_.Broadcast(cmd->p_param, cmd->p_size);
          break;
        }
        }
			}
			this_thread::sleep_for(chrono::milliseconds(1));
		}
		sessions_.CloseAllSessions();
		cout << "TcpServer:ThreadProc End" << endl;
	}

	//void OnAccepted(shared_ptr<TcpConnection> connection) {
  void OnAccepted(shared_ptr<tcp::socket> sock) {
    auto connection = make_shared<ServerConnection>(shared_from_this(), sock);
    

		int32_t session_id = sessions_.GetUnusedSessionId();
		if (session_id == -1) {
			//todo: send prompting message
			connection->Close();
		} else {
      connection->SetSessionId(session_id);
			//connection->SetHandler_OnPacket(boost::bind(&TcpServer::OnPacket, this, session_id, _1, _2));
			connection->SetHandler_OnClose(boost::bind(&TcpServer::OnClose, this, session_id, _1));
			connection->Start();
			sessions_.SetSession(session_id, connection);
			OnNewSession(session_id);
		}
	}
	//void OnPacket(int32_t session_id, uint8_t *ptr, uint32_t size) {
	//	OnSessionPacket(session_id, ptr, size);
	//}
	void OnClose(int32_t session_id, int32_t reason) {
		//cout << "TcpServer::OnClose(" << session_id << ", " << reason << ")" << endl;
		sessions_.ReleaseSession(session_id);
		OnSessionClosed(session_id, reason);
	}

private:
	boost::asio::io_service io_service_;
	Sessions sessions_;
};