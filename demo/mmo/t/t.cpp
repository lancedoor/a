// t.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <thread>
using namespace std;
#include "../frame/Frame/ClientFrame.h"
#include "../frame/Frame/ServerFrame.h"
#include "../frame/NetActor/ClientNetActor.h"

#include "../frame/Net/TcpServer.h"
#include "../frame/Net/TcpClient.h"
#include "../common/Packets.pb.h"

#pragma comment(lib, "libprotobuf.lib")
#pragma comment(lib, "frame.lib")
#pragma comment(lib, "common.lib")

class MyClientNetActor : public ClientNetActor {
public:
  virtual void Init() {
    ClientNetActor::Init();
    REGISTER_PACKET_HANDLER(MyClientNetActor, Packet::SC_SomeoneSay, OnPacket_SC_SomeoneSay);
  }
private:
  void OnPacket_SC_SomeoneSay(shared_ptr<::google::protobuf::Message> _packet) {
    auto packet = dynamic_pointer_cast<Packet::SC_SomeoneSay>(_packet);
    if (!packet)
      return;
    cout << packet->name() + ": " + packet->text() + "\n";
  }

};

class Broker : public ConnectionActor {
public:
  virtual void Init() {
    ConnectionActor::Init();
    REGISTER_PACKET_HANDLER(Broker, Packet::CS_Say, OnPacket_CS_Say);
  }

private:
  void OnPacket_CS_Say(shared_ptr<::google::protobuf::Message> _packet) {
    auto packet = dynamic_pointer_cast<Packet::CS_Say>(_packet);
    if (!packet)
      return;
    cout << to_string(GetSessionId()) + ": " + packet->text() + "\n";
  }

};

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
  ClientFrame::Get()->Init(1, make_shared<MyClientNetActor>());
  ClientFrame::Get()->Start();

  ServerFrame::Get()->Init(2, []()->shared_ptr<ConnectionActor> {return make_shared<Broker>();});
  ServerFrame::Get()->Start();

  //auto server = make_shared<MyTcpServer>();
  //server->Start();

  //auto client = make_shared<MyTcpClient >();
  //client->Start();

  for (;;) {
    //server->Poll();
    //client->Poll();
    this_thread::sleep_for(chrono::seconds(1));

    auto packet = make_shared<Packet::CS_Say>();
    packet->set_text("client say");
    ClientFrame::Get()->SendPacket(packet);
    //client->SendPacket(packet);

    auto server_packet = make_shared<Packet::SC_SomeoneSay>();
    server_packet->set_name("Server");
    server_packet->set_text("Welcome");
    //server->SendPacket(0, server_packet);
    ServerFrame::Get()->BroadcastPacket(server_packet);
  }

  //client->Stop();
  //server->Stop();

  ClientFrame::Get()->Stop();
  ServerFrame::Get()->Stop();

    return 0;
}

