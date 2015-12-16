#pragma once
#include <memory>
using namespace std;
#include "TcpConnector.h"
#include "TcpConnection.h"

class TcpClient : public enable_shared_from_this<TcpClient> {
  class Connector : public TcpConnector {
  public:
    Connector(weak_ptr<TcpClient> tcp_client, boost::asio::io_service &io_service)
      : TcpConnector(io_service) {
      tcp_client_ = tcp_client;
    }
  protected:
    void OnConnected(shared_ptr<tcp::socket> sock, const boost::system::error_code &ec) {
      auto p = tcp_client_.lock();
      if (p)
        p->_OnConnected(sock, ec);
    }
  private:
    weak_ptr<TcpClient> tcp_client_;
  };
  class Connection : public TcpPacketConnection {
  public:
    Connection(weak_ptr<TcpClient> tcp_client, shared_ptr<tcp::socket> _socket)
      : TcpPacketConnection(_socket) {
      tcp_client_ = tcp_client;
    }
  private:
    virtual void OnRecvPacket(shared_ptr<::google::protobuf::Message> packet) {
      auto p = tcp_client_.lock();
      if (p)
        p->OnPacket(packet);
    }
    virtual void OnClosed(int32_t reason) {
      auto p = tcp_client_.lock();
      if (p)
        p->OnClosed(reason);
    }
  private:
    weak_ptr<TcpClient> tcp_client_;
  };
public:
	TcpClient() {
	}
	~TcpClient() {
	}

  // Start & Stop & Poll
  void Start() {
    connector_ = make_shared<Connector>(shared_from_this(), io_service_);
    connector_->Connect();
  }
	void Stop() {
    if (connection_)
      connection_->Close();
		io_service_.stop();
	}
  void Poll() {
    io_service_.poll();
  }

  // SendPacket
  void SendPacket(shared_ptr<::google::protobuf::Message> packet) {
    if (connection_)
      connection_->SendPacket(packet);
  }
protected: // Events
  virtual void OnConnected(const boost::system::error_code &ec) {}
  virtual void OnPacket(shared_ptr<::google::protobuf::Message> packet) {}
  virtual void OnClosed(int32_t reason) {}


private:
  void _OnConnected(shared_ptr<tcp::socket> sock, const boost::system::error_code &ec) {
    if (!ec) {
      connection_ = make_shared<Connection>(shared_from_this(), sock);
      connection_->Start();
    }
    OnConnected(ec);
  }
private:
	boost::asio::io_service io_service_;
  shared_ptr<Connector> connector_;
  shared_ptr<Connection> connection_;
};