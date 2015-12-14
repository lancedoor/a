#pragma once
#include "NetThread.h"
#include "MessageManager.h"
#include "ClientConnection.h"
#include "TcpConnector.h"
using namespace std;

class TcpClient : public NetThread, public enable_shared_from_this<TcpClient> {
  //enum ECmdId {
  //  SEND
  //};
public:
	TcpClient() {
	}
	~TcpClient() {
	}
	void Stop() {
    Thread::Stop();
		io_service_.stop();
		join();
	}
  void SendPacket(shared_ptr<::google::protobuf::Message> packet) {
    auto msg = make_shared<NetThreadMsg>(NetThreadMsg::ECmdId::SEND, 0, packet);
    msg_q_.PushBack(msg);
  }
  virtual void OnPacket(shared_ptr<::google::protobuf::Message> packet) {}

private:
	virtual void ThreadProc() {
		auto sock = TcpConnector::Connect(io_service_);
		if (!sock)
			return;

    auto conn = make_shared<ClientConnection>(shared_from_this(), sock);

		//conn->SetHandler_OnPacket(boost::bind(&TcpClient::OnPacket, this, _1, _2));
		conn->SetHandler_OnClose(boost::bind(&TcpClient::OnClose, this, _1));
		conn->Start();
		while(!stop_) {
			io_service_.poll();

			auto msg = msg_q_.PopFront();
			if (msg) {
        switch (msg->cmd_id)
        {
        case NetThreadMsg::ECmdId::SEND:
          conn->SendPacket(msg->packet);
        default:
          break;
        }
			}
			this_thread::sleep_for(chrono::milliseconds(1));
		}
		conn->Close();
		cout << "TcpClient:ThreadProc End" << endl;
	}
	void OnPacket(uint8_t *ptr, uint32_t size) {
    string s((const char*)ptr, size);
		cout << s << endl;
	}
	void OnClose(int32_t reason) {
		cout << "TcpClient::OnClose(" << reason << ")" << endl;
	}

private:
	boost::asio::io_service io_service_;
};