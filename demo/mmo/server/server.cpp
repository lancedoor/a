// server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <memory>
using namespace std;

#include "../frame/Frame/ServerFrame.h"
#include "../common/Packets.pb.h"
#include "MyServerNetActor.h"

#pragma comment(lib, "libprotobuf.lib")
#pragma comment(lib, "frame.lib")
#pragma comment(lib, "common.lib")

int main()
{
  ServerFrame::Get()->Init(2, make_shared<MyServerNetActor>());
  ServerFrame::Get()->Start();

	//auto tcp_server = make_shared<MyTcpServer>();

	//int32_t receptionist_id = ActorManager::Get()->AddActor(make_shared<Receptionist>(tcp_server));
	//tcp_server->SetReceptionistId(receptionist_id);

 // WorkerThreadManager::Get()->Start();
  //Threads<ActorThread, 2> threads;

	//tcp_server->Start();
	for (;;) {
    char c[256];
    cin.getline(c, 256);
		string s(c);
		//cin >> s;


		if (s == "exit")
			break;

    auto packet = make_shared<Packet::SC_SomeoneSay>();
    packet->set_name("System");
    packet->set_text(s);
    //tcp_server->BroadcastPacket(packet);
    ServerFrame::Get()->BroadcastPacket(packet);
	}
	//tcp_server->Stop();
	//WorkerThreadManager::Get()->Stop();

  ServerFrame::Get()->Stop();
	return 0;
}

