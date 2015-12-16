#pragma once
#include <boost/asio.hpp>
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
  TcpConnection(shared_ptr<tcp::socket> _socket)
    : socket_(_socket)
    , closed_(false) {
  }
	~TcpConnection() {

	}
	void Start() {
		RecvHeader();
	}
  void Send(shared_ptr<uint8_t> data, int32_t size) {
    if (closed_)
      return;

    boost::system::error_code ignored_error;
    boost::asio::write(*socket_, boost::asio::buffer(&size, sizeof(size)), ignored_error);
    boost::asio::write(*socket_, boost::asio::buffer(data.get(), size), ignored_error);
  }

	void Close() {
		if (closed_)
			return;

		InternalClose(CLOSEREASON_ACTIVE);
	}
protected:
  virtual void OnRecv(uint8_t* ptr, uint32_t size){}
  virtual void OnClosed(int32_t reason) {}
private:
	void RecvHeader() {
		boost::asio::async_read(*socket_,
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

		boost::asio::async_read(*socket_,
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
    OnRecv(recv_buffer_, bytes_transferred);
		RecvHeader();
	}
	void InternalClose(int32_t reason) {
		socket_->shutdown(tcp::socket::shutdown_both);
		socket_->close();

		closed_ = true;
    OnClosed(reason);
	}

private:
  shared_ptr<tcp::socket> socket_;
	uint32_t header_;
	uint8_t recv_buffer_[RECV_BUFFER_SIZE];
	bool closed_;
};

