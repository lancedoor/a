// server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "../net/all.h"
#include "../net/TcpServer.h"
#include "../net/MessageManager.h"
#include "../net/WorkerThread.h"
#include "Receptionist.h"
#include "MyTcpServer.h"
#include "../net/WorkerThreadManager.h"

#pragma comment(lib, "net.lib")
#pragma comment(lib, "common.lib")
#pragma comment(lib, "libprotobuf.lib")

int main()
{
	auto tcp_server = make_shared<MyTcpServer>();

	int32_t receptionist_id = ActorManager::Get()->AddActor(make_shared<Receptionist>(tcp_server));
	tcp_server->SetReceptionistId(receptionist_id);

	WorkerThreadManager::Get()->Start();

	tcp_server->Start();
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
    tcp_server->BroadcastPacket(packet);
		//tcp_server->PutCmd(unique_ptr<string>(new string(s)));
	}
	tcp_server->Stop();
	WorkerThreadManager::Get()->Stop();
	return 0;
}

