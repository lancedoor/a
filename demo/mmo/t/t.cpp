// t.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <thread>
using namespace std;
#include "../frame/Net/TcpServer.h"
#include "../frame/Net/TcpClient.h"
#include "../common/Packets.pb.h"

#pragma comment(lib, "libprotobuf.lib")
#pragma comment(lib, "frame.lib")
#pragma comment(lib, "common.lib")


class MyTcpServer : public TcpServer {
protected:
  virtual void OnNewSession(int32_t session_id) {
    cout << "MyTcpServer::OnNewSession" << endl;
  }
  virtual void OnSessionPacket(int32_t session_id, shared_ptr<::google::protobuf::Message> packet) {
    cout << "MyTcpServer::OnSessionPacket" << endl;
  }
  virtual void OnSessionClosed(int32_t session_id, int32_t reason) {
    cout << "MyTcpServer::OnSessionClosed" << endl;
  }

};

class MyTcpClient : public TcpClient {
protected: // Events
  virtual void OnConnected(const boost::system::error_code &ec) {
    cout << "MyTcpClient::OnConnected" << endl;
  }
  virtual void OnPacket(shared_ptr<::google::protobuf::Message> packet) {
    cout << "MyTcpClient::OnPacket" << endl;
  }
  virtual void OnClosed(int32_t reason) {
    cout << "MyTcpClient::OnClosed" << endl;
  }
};

int main()
{
  auto server = make_shared<MyTcpServer>();
  server->Start();

  auto client = make_shared<MyTcpClient >();
  client->Start();

  for (;;) {
    server->Poll();
    client->Poll();
    this_thread::sleep_for(chrono::seconds(1));

    auto packet = make_shared<Packet::CS_Say>();
    packet->set_text("client say");
    client->SendPacket(packet);

    packet = make_shared<Packet::CS_Say>();
    packet->set_text("server say");
    server->SendPacket(0, packet);
  }

  client->Stop();
  server->Stop();

    return 0;
}

