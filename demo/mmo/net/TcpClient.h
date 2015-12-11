#pragma once
#include "Thread.h"
#include "MessageManager.h"
#include "Packet.h"
using namespace std;

class TcpClient : public Thread {
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
  void SendPacket(shared_ptr<Packet> packet) {
    auto cmd = make_shared<Cmd>();
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
    //    auto p = shared_ptr<uint8_t>(new uint8_t[cmd->p_size + 4]);
    //    (*(int32_t*)p.get()) = 0;
    //    memcpy(p.get() + 4, cmd->p_param.get(), cmd->p_size);
				//conn->Send(p, cmd->p_size + 4);
        conn->Send(cmd->p_param, cmd->p_size);
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