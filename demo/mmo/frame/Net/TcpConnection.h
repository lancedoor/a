#pragma once
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include "DynamicBuffer.h"
#include "SendBuffer.h"

using boost::asio::ip::tcp;
class TcpConnection : public enable_shared_from_this<TcpConnection>
{
public:
	enum ECloseReason {
		Active,
		Passive,
		Timeout,
    RecvBufferRunOut,
    ErrorInRecv,
    SendBufferRunOut,
    ErrorInSend,

		_Max
	};
	enum {
    kMinRecvBuffer = 4 * 1024,
    kMaxRecvBuffer = 8 * 1024 * 1024,
    kMinSendBuffer = 4 * 1024,
    kMaxSendBuffer = 8 * 1024 * 1024,
  };
public:
  TcpConnection(shared_ptr<tcp::socket> _socket)
  : socket_(_socket)
  , closed_(false)
  , recv_buffer_(kMinRecvBuffer, kMaxRecvBuffer)
  , send_buffer_a_(kMinSendBuffer, kMaxSendBuffer)
  , send_buffer_b_(kMinSendBuffer, kMaxSendBuffer) {
    sending_ = false;
    sending_buffer_ = &send_buffer_a_;
    pending_buffer_ = &send_buffer_b_;
  }
	~TcpConnection() {

	}
	void Start() {
		RecvHeader();
	}
  void Send(shared_ptr<uint8_t> data, int32_t size) {
    if (closed_)
      return;

    if (!pending_buffer_->Append((uint8_t*)&size, sizeof(size))) {
      InternalClose(ECloseReason::SendBufferRunOut);
      return;
    }
    if (!pending_buffer_->Append(data.get(), size)) {
      InternalClose(ECloseReason::SendBufferRunOut);
      return;
    }
      
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

    if (!recv_buffer_.MakeRoom(header_, 0)) {
      InternalClose(ECloseReason::RecvBufferRunOut);
      return;
    }
    recv_buffer_.SaveRoom(header_, 0);

		boost::asio::async_read(*socket_,
			boost::asio::buffer(recv_buffer_.GetBufPtr(), header_),
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
    OnRecv(recv_buffer_.GetBufPtr(), bytes_transferred);
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
                      boost::asio::buffer(sending_buffer_->GetBufPtr(), sending_buffer_->GetDataSize()),
                      boost::bind(&TcpConnection::AsyncSendHandler, shared_from_this(), _1, _2));
    sending_ = true;
  }
  void AsyncSendHandler(const boost::system::error_code& ec, std::size_t bytes_transferred) {
    sending_ = false;
    if (closed_)
      return;
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
  DynamicBuffer recv_buffer_;

	bool closed_;

  bool sending_;
  SendBuffer send_buffer_a_;
  SendBuffer send_buffer_b_;
  SendBuffer* sending_buffer_;
  SendBuffer* pending_buffer_;

};

