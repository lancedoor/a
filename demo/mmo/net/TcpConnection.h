#pragma once
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

using boost::asio::ip::tcp;
class TcpConnection : public enable_shared_from_this<TcpConnection>
{
public:
	enum CloseReason {
		CLOSEREASON_ACTIVE,
		CLOSEREASON_PASSIVE,
		CLOSEREASON_TIMEOUT,

		_CLOSEREASON_MAX
	};
	enum {
		RECV_BUFFER_SIZE = 1024
	};
public:
	TcpConnection(boost::asio::io_service &io_service)
		: socket_(io_service)
		, closed_(false) {
	}
	~TcpConnection() {

	}
  tcp::socket &Socket() {
    return socket_;
  }

	void SetHandler_OnPacket(const boost::function<void(uint8_t *ptr, uint32_t size)> &fn) {
		handler_on_packet_ = fn;
	}
	void SetHandler_OnClose(const boost::function<void(int32_t reason)> &fn) {
		handler_on_close_ = fn;
	}

	void Start() {
		WaitHeader();
	}

	void Send(const string &s) {
		if (closed_)
			return;

		boost::system::error_code ignored_error;
		uint32_t l = s.size();
		boost::asio::write(socket_, boost::asio::buffer(&l, sizeof(l)), ignored_error);
		boost::asio::write(socket_, boost::asio::buffer(s), ignored_error);
	}
  void Send(shared_ptr<uint8_t> data, int32_t size) {
    if (closed_)
      return;

    boost::system::error_code ignored_error;
    boost::asio::write(socket_, boost::asio::buffer(&size, sizeof(size)), ignored_error);
    boost::asio::write(socket_, boost::asio::buffer(data.get(), size), ignored_error);
  }

	void Close() {
		if (closed_)
			return;

		InternalClose(CLOSEREASON_ACTIVE);
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
			InternalClose(CLOSEREASON_PASSIVE);
			return;
		}
		if (header_ > RECV_BUFFER_SIZE) {
			InternalClose(CLOSEREASON_ACTIVE); //todo: more detail
			return;
		}

		boost::asio::async_read(socket_,
			boost::asio::buffer(recv_buffer_, header_),
			boost::bind(&TcpConnection::OnRecvBody, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred)
			);
	}
	void OnRecvBody(const boost::system::error_code& error, size_t bytes_transferred) {
		if (error || bytes_transferred == 0) {
			InternalClose(CLOSEREASON_PASSIVE);
			return;
		}
		if (handler_on_packet_) {
      handler_on_packet_(recv_buffer_, bytes_transferred);
		}

		WaitHeader();
	}
	void InternalClose(int32_t reason) {
		socket_.shutdown(tcp::socket::shutdown_both);
		socket_.close();

		closed_ = true;
		if (handler_on_close_)
			handler_on_close_(reason);
	}

private:
	tcp::socket socket_;
	uint32_t header_;
	uint8_t recv_buffer_[RECV_BUFFER_SIZE];
	boost::function<void(uint8_t *ptr, uint32_t size)> handler_on_packet_;
	boost::function<void(int32_t reason)> handler_on_close_;
	bool closed_;
};

