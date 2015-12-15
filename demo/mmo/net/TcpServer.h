#pragma once
#include <boost/asio.hpp>
#include "NetThread.h"
#include "MessageManager.h"
#include "TcpAcceptor.h"
#include "Sessions.h"
using namespace std;

#include "ServerConnection.h"

class TcpServer : public NetThread, public enable_shared_from_this<TcpServer> {
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
  void SendPacket(int32_t session_id, shared_ptr<::google::protobuf::Message> packet) {
    msg_q_.PushBack(make_shared<NetThreadMsg>(NetThreadMsg::ECmdId::SEND, session_id, packet));
  }
  void BroadcastPacket(shared_ptr<::google::protobuf::Message> packet) {
    msg_q_.PushBack(make_shared<NetThreadMsg>(NetThreadMsg::ECmdId::BROADCAST, 0, packet));
  }
public:
  virtual void OnSessionPacket(int32_t session_id, shared_ptr<::google::protobuf::Message> packet) {}
private:
	virtual void OnNewSession(int32_t session_id) {}
	virtual void OnSessionClosed(int32_t session_id, int32_t reason) {}
private:
	virtual void ThreadProc() {

    auto acceptor = make_shared<TcpAcceptor>(io_service_);
		acceptor->SetHandler_OnAccepted(boost::bind(&TcpServer::OnAccepted, this, _1));
		acceptor->Start();

		while (!stop_) {
			io_service_.poll();

      auto msg = msg_q_.PopFront();
			if (msg) {
        switch (msg->cmd_id) {
        case NetThreadMsg::ECmdId::SEND: {
          auto session = sessions_.GetSession(msg->session_id);
          if (session)
            session->SendPacket(msg->packet);
          break;
        }
        case NetThreadMsg::ECmdId::BROADCAST: {
          sessions_.Broadcast(msg->packet);
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
	void OnClose(int32_t session_id, int32_t reason) {
		//cout << "TcpServer::OnClose(" << session_id << ", " << reason << ")" << endl;
		sessions_.ReleaseSession(session_id);
		OnSessionClosed(session_id, reason);
	}

private:
	boost::asio::io_service io_service_;
	Sessions<ServerConnection> sessions_;
};