#pragma once

#include "TcpConnection.h"

using boost::asio::ip::tcp;
class TcpAcceptor : public enable_shared_from_this<TcpAcceptor> {
public:
	TcpAcceptor(boost::asio::io_service &io_service)
	: acceptor_(io_service, tcp::endpoint(tcp::v4(), 55555)) {
	}
	~TcpAcceptor() {

	}
	//void SetHandler_OnAccepted(const boost::function<void(shared_ptr<TcpConnection>)> &fn) {
  void SetHandler_OnAccepted(const boost::function<void(shared_ptr<tcp::socket>)> &fn) {
		handler_on_accepted_ = fn;
	}
	void Start() {
		WaitConnection();
	}

private:
	void WaitConnection() {
		//shared_ptr<TcpConnection> tcp_connection = make_shared<TcpConnection>(acceptor_.get_io_service());
		//acceptor_.async_accept(tcp_connection->Socket(), boost::bind(&TcpAcceptor::OnAccept, this, tcp_connection, boost::asio::placeholders::error));
    auto sock = make_shared<tcp::socket>(acceptor_.get_io_service());
    acceptor_.async_accept(*sock, boost::bind(&TcpAcceptor::AcceptHandler, shared_from_this(), sock, boost::asio::placeholders::error));
  }
	//void AcceptHandler(shared_ptr<TcpConnection> tcp_connection, const boost::system::error_code &error) {
  void AcceptHandler(shared_ptr<tcp::socket> sock, const boost::system::error_code &error) {
		if (!error)
		{
			if (handler_on_accepted_)
				//handler_on_accepted_(tcp_connection);
        handler_on_accepted_(sock);
		}

		WaitConnection();
	}
private:
	tcp::acceptor acceptor_;
	//boost::function<void(shared_ptr<TcpConnection>)> handler_on_accepted_;
  boost::function<void(shared_ptr<tcp::socket>)> handler_on_accepted_;
};