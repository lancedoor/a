#pragma once
#include <memory>
using namespace std;
#include <boost/bind.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
class TcpAcceptor : public enable_shared_from_this<TcpAcceptor> {
public:
	TcpAcceptor(boost::asio::io_service &io_service)
	: acceptor_(io_service, tcp::endpoint(tcp::v4(), 55555)) {
	}
	~TcpAcceptor() {
	}
	void Start() {
		WaitConnection();
	}
  void Stop() {
    acceptor_.close();
  }
protected:
  virtual void OnAccepted(shared_ptr<tcp::socket> sock) {}
private:
	void WaitConnection() {
    auto sock = make_shared<tcp::socket>(acceptor_.get_io_service());
    acceptor_.async_accept(*sock, boost::bind(&TcpAcceptor::AcceptHandler, shared_from_this(), sock, boost::asio::placeholders::error));
  }
  void AcceptHandler(shared_ptr<tcp::socket> sock, const boost::system::error_code &error) {
		if (!error)
      OnAccepted(sock);

		WaitConnection();
	}
private:
	tcp::acceptor acceptor_;
};