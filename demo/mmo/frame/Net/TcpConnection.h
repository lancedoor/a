#pragma once
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include "DynamicBuffer.h"

using boost::asio::ip::tcp;
class TcpConnection : public enable_shared_from_this<TcpConnection>
{
public:
	enum ECloseReason {
		Active,
		Passive,
		Timeout,
    ErrorInSend,

		_Max
	};
	enum {
		RECV_BUFFER_SIZE = 1024,
	};
public:
  TcpConnection(shared_ptr<tcp::socket> _socket)
    : socket_(_socket)
    , closed_(false) {
    sending_ = false;
    sending_buffer_ = &send_buffer_[0];
    pending_buffer_ = &send_buffer_[1];
  }
	~TcpConnection() {

	}
	void Start() {
		RecvHeader();
	}
  void Send(shared_ptr<uint8_t> data, int32_t size) {
    if (closed_)
      return;

    pending_buffer_->Append((uint8_t*)&size, sizeof(size));
    pending_buffer_->Append(data.get(), size);
    if (!sending_)
      LaunchAsyncWrite();
  }

	void Close() {
		if (closed_)
			return;

		InternalClose(ECloseReason::Active);
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
			InternalClose(ECloseReason::Passive);
			return;
		}
		if (header_ > RECV_BUFFER_SIZE) {
			InternalClose(ECloseReason::Active); //todo: more detail
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
			InternalClose(ECloseReason::Passive);
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

  void LaunchAsyncWrite() {
    assert(!sending_);
    if (sending_)
      return;
    if (pending_buffer_->GetDataSize() <= 0)
      return;

    swap(sending_buffer_, pending_buffer_);
    boost::asio::async_write(*socket_,
                      boost::asio::buffer(sending_buffer_->GetDataPtr(), sending_buffer_->GetDataSize()),
                      boost::bind(&TcpConnection::AsyncSendHandler, shared_from_this(), _1, _2));
    sending_ = true;
  }
  void AsyncSendHandler(const boost::system::error_code& ec, std::size_t bytes_transferred) {
    sending_ = false;
    if (ec) {
      InternalClose(ECloseReason::ErrorInSend);
      return;
    }

    sending_buffer_->Clear();
    if (pending_buffer_->GetDataSize() > 0) {
      LaunchAsyncWrite();
    }
  }

private:
  shared_ptr<tcp::socket> socket_;
	uint32_t header_;
	uint8_t recv_buffer_[RECV_BUFFER_SIZE];
	bool closed_;

  bool sending_;
  DynamicBuffer send_buffer_[2];
  DynamicBuffer* sending_buffer_;
  DynamicBuffer* pending_buffer_;

};

