// client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "../net/Threads.h"
#include "../net/ActorThread.h"
#include "../net/ActorMgr.h"
#include "../common/Packets.pb.h"
#include "MyTcpClient.h"
#include "Actor_Server.h"
#include "UserInfo.h"

#pragma comment(lib, "net.lib")
#pragma comment(lib, "common.lib")
#pragma comment(lib, "libprotobuf.lib")

int main()
{
  Threads<ActorThread, 1> threads;
  auto actor_server_id = ActorMgr::Get()->AddActor(make_shared<Actor_Server>());

	auto tcp_client = make_shared<MyTcpClient>(actor_server_id);
	tcp_client->Start();

  auto packet = make_shared<Packet::CS_Login>();
  packet->set_user("guest");
  tcp_client->SendPacket(packet);

  for (;;) {
    //cout << UserInfo::Get()->GetName() << ":";
    char c[256];
    cin.getline(c, 256);
		string s(c);

		if (s == "exit")
			break;

    auto packet = make_shared<Packet::CS_Say>();
    packet->set_text(s);
    tcp_client->SendPacket(packet);
	}
	tcp_client->Stop();

  return 0;
}

