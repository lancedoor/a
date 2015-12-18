#pragma once
#include <boost/asio.hpp>
#include "TcpAcceptor.h"
#include "TcpPacketConnection.h"
#include "../Util/IdPtrMap.h"
using namespace std;

class TcpServer : public enable_shared_from_this<TcpServer> {
  class Acceptor : public TcpAcceptor {
  public:
    Acceptor(weak_ptr<TcpServer> tcp_server, boost::asio::io_service &io_service) 
    : TcpAcceptor(io_service) {
      tcp_server_ = tcp_server;
    }
  protected:
    virtual void OnAccepted(shared_ptr<tcp::socket> sock) {
      auto p = tcp_server_.lock();
      if (p)
        p->OnAccepted(sock);
    }
  private:
    weak_ptr<TcpServer> tcp_server_;
  };
  class Connection : public TcpPacketConnection {
  public:
    Connection(weak_ptr<TcpServer> tcp_server, shared_ptr<tcp::socket> _socket)
      : TcpPacketConnection(_socket) {
      tcp_server_ = tcp_server;
    }
    void SetSessionId(int32_t id) {
      session_id_ = id;
    }
  private:
    virtual void OnRecvPacket(shared_ptr<::google::protobuf::Message> packet) {
      auto p = tcp_server_.lock();
      if (p)
        p->OnSessionPacket(session_id_, packet);
    }
    virtual void OnClosed(int32_t reason) {
      auto p = tcp_server_.lock();
      if (p)
        p->OnConnectionClosed(session_id_, reason);
    }
  private:
    weak_ptr<TcpServer> tcp_server_;
    int32_t session_id_;
  };

public:
	TcpServer()
	: sessions_(2) {
	}
	~TcpServer() {
	}
  int32_t GetMaxSessionCount() {
    return 2;
  }

  void Start() {
    acceptor_ = make_shared<Acceptor>(shared_from_this(), io_service_);
    acceptor_->Start();
  }
  void Poll() {
      io_service_.poll();
  }
	void Stop() {
    acceptor_->Stop();
    ClearSessions();
    io_service_.stop();
	}
  void SendPacket(int32_t session_id, shared_ptr<::google::protobuf::Message> packet) {
    auto conn = sessions_.GetItem(session_id);
    if (conn)
      conn->SendPacket(packet);
  }
  void BroadcastPacket(shared_ptr<::google::protobuf::Message> packet) {
    for (auto &item : sessions_) {
      if (item.second)
        item.second->SendPacket(packet);
    }
  }
  void CloseSession(int32_t session_id) {
    auto conn = sessions_.GetItem(session_id);
    if (conn)
      conn->Close();
  }
protected:
	virtual void OnNewSession(int32_t session_id) {}
  virtual void OnSessionPacket(int32_t session_id, shared_ptr<::google::protobuf::Message> packet) {}
  virtual void OnSessionClosed(int32_t session_id, int32_t reason) {}
private:
  void OnAccepted(shared_ptr<tcp::socket> sock) {
    auto connection = make_shared<Connection>(shared_from_this(), sock);

    if (sessions_.IsFull()) {
      //todo: send prompting message
      connection->Close();
    } else {
      int32_t session_id = sessions_.AddItem(connection);
      connection->SetSessionId(session_id);
      OnNewSession(session_id);
      connection->Start();
    }

	}
	void OnConnectionClosed(int32_t session_id, int32_t reason) {
    sessions_.erase(session_id);
		OnSessionClosed(session_id, reason);
	}

  void ClearSessions() {
    for (auto &item : sessions_) {
      if (item.second)
        item.second->Close();
    }
    sessions_.clear();
  }
private:
	boost::asio::io_service io_service_;
  shared_ptr<Acceptor> acceptor_;
  IdPtrMap<Connection> sessions_;
};