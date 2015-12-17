// client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
//#include "../net/Threads.h"
//#include "../net/ActorThread.h"
//#include "../net/ActorMgr.h"
#include "../frame/NetActor/ClientNetActor.h"
#include "../frame/Frame/ClientFrame.h"
#include "../common/Packets.pb.h"
//#include "MyTcpClient.h"
//#include "Actor_Server.h"
#include "UserInfo.h"

//#pragma comment(lib, "net.lib")
#pragma comment(lib, "frame.lib")
#pragma comment(lib, "common.lib")
#pragma comment(lib, "libprotobuf.lib")

class MyClientNetActor : public ClientNetActor {
public:
  MyClientNetActor() {
    RegisterPacketHandler(PacketType(make_shared<Packet::SC_LoginResult>()), boost::bind(&MyClientNetActor::OnPacket_SC_LoginResult, this, _1));
    RegisterPacketHandler(PacketType(make_shared<Packet::SC_SomeoneSay>()), boost::bind(&MyClientNetActor::OnPacket_SC_SomeoneSay, this, _1));
  }
private:
  void OnPacket_SC_LoginResult(shared_ptr<::google::protobuf::Message> _packet) {
    auto packet = dynamic_pointer_cast<Packet::SC_SomeoneSay>(_packet);
    if (!packet)
      return;
    cout << packet->name() + ": " + packet->text() + "\n";
  }
  void OnPacket_SC_SomeoneSay(shared_ptr<::google::protobuf::Message> _packet) {
    auto packet = dynamic_pointer_cast<Packet::SC_SomeoneSay>(_packet);
    if (!packet)
      return;
    cout << packet->name() + ": " + packet->text() + "\n";
  }
};


int main()
{
  ClientFrame::Get()->Init(1, make_shared<MyClientNetActor>());
  ClientFrame::Get()->Start();

 // Threads<ActorThread, 1> threads;
 // auto actor_server_id = ActorMgr::Get()->AddActor(make_shared<Actor_Server>());

	//auto tcp_client = make_shared<MyTcpClient>(actor_server_id);
	//tcp_client->Start();

  auto packet = make_shared<Packet::CS_Login>();
  packet->set_user("guest");
  //tcp_client->SendPacket(packet);
  ClientFrame::Get()->SendPacket(packet);

  for (;;) {
    //cout << UserInfo::Get()->GetName() << ":";
    char c[256];
    cin.getline(c, 256);
		string s(c);

		if (s == "exit")
			break;

    auto packet = make_shared<Packet::CS_Say>();
    packet->set_text(s);
    //tcp_client->SendPacket(packet);
    ClientFrame::Get()->SendPacket(packet);
	}
	//tcp_client->Stop();

  ClientFrame::Get()->Stop();
  return 0;
}

