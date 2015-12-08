#pragma once
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

using boost::asio::ip::tcp;
class TcpConnection : public enable_shared_from_this<TcpConnection>
{
	friend class TcpAcceptor;
	friend class TcpConnector;
public:
	TcpConnection(boost::asio::io_service &io_service)
		: socket_(io_service) {
	}
	~TcpConnection() {

	}

	void SetHandler_OnPacket(const boost::function<void(const string &s)> &fn) {
		handler_on_packet_ = fn;
	}

	void Start() {
		WaitHeader();
	}

	void Send(const string &s) {
		boost::system::error_code ignored_error;
		uint32_t l = s.size();
		boost::asio::write(socket_, boost::asio::buffer(&l, sizeof(l)), ignored_error);
		boost::asio::write(socket_, boost::asio::buffer(s), ignored_error);
	}

	void Close() {
		socket_.shutdown(tcp::socket::shutdown_both);
		socket_.close();
	}
private:
	void WaitHeader() {
		boost::asio::async_read(socket_,
			boost::asio::buffer(&header_, sizeof(header_)),
			boost::bind(&TcpConnection::OnRecvHeader, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred)
			);
	}
	void OnRecvHeader(const boost::system::error_code& error, size_t bytes_transferred) {
		if (error || bytes_transferred == 0) {
			Close();
			return;
		}
		//todo: async_recv(body)
		char s[1024];
		boost::asio::read(socket_, boost::asio::buffer(s, header_));
		s[header_] = 0;
		if (handler_on_packet_) {
			handler_on_packet_(s);
		}

		WaitHeader();
	}
	void OnRecvBody() {

	}

private:
	tcp::socket socket_;
	uint32_t header_;
	boost::function<void(const string &s)> handler_on_packet_;
};

