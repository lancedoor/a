#pragma once
#include "Thread.h"
#include "MessageManager.h"
#include "Packet.h"
using namespace std;

class TcpClient : public Thread {
  enum ECmdId {
    SEND
  };
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
    auto cmd = make_shared<Cmd>(ECmdId::SEND, 0, nullptr, 0);
    GetPacketBlock(packet, cmd->p_param, cmd->p_size);
    PutCmd(cmd);
  }
private:
	virtual void ThreadProc() {
		auto conn = TcpConnector::Connect(io_service_);
		if (!conn)
			return;

		conn->SetHandler_OnPacket(boost::bind(&TcpClient::OnPacket, this, _1, _2));
		conn->SetHandler_OnClose(boost::bind(&TcpClient::OnClose, this, _1));
		conn->Start();
		while(!stop_) {
			io_service_.poll();

			auto cmd = GetCmd();
			if (cmd) {
        switch (cmd->id)
        {
        case ECmdId::SEND:
          conn->Send(cmd->p_param, cmd->p_size);
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