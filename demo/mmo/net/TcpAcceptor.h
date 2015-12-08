#pragma once

#include "TcpConnection.h"

using boost::asio::ip::tcp;
class TcpAcceptor {
public:
	TcpAcceptor(boost::asio::io_service &io_service)
	: acceptor_(io_service, tcp::endpoint(tcp::v4(), 55555)) {
	}
	~TcpAcceptor() {

	}
	void SetHandler_OnAccepted(const boost::function<void(shared_ptr<TcpConnection>)> &fn) {
		handler_on_accepted_ = fn;
	}
	void Start() {
		WaitConnection();
	}

private:
	void WaitConnection() {
		shared_ptr<TcpConnection> tcp_connection = make_shared<TcpConnection>(acceptor_.get_io_service());
		acceptor_.async_accept(tcp_connection->socket_, boost::bind(&TcpAcceptor::OnAccept, this, tcp_connection, boost::asio::placeholders::error));
	}
	void OnAccept(shared_ptr<TcpConnection> tcp_connection, const boost::system::error_code &error) {
		if (!error)
		{
			if (handler_on_accepted_)
				handler_on_accepted_(tcp_connection);
		}

		WaitConnection();
	}
private:
	tcp::acceptor acceptor_;
	boost::function<void(shared_ptr<TcpConnection>)> handler_on_accepted_;
};