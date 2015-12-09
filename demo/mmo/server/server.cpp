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

int main()
{
	auto tcp_server = make_shared<MyTcpServer>();

	int32_t receptionist_id = ActorManager::Get()->AddActor(make_shared<Receptionist>(tcp_server));
	tcp_server->SetReceptionistId(receptionist_id);

	WorkerThreadManager::Get()->Start();

	tcp_server->Start();
	for (;;) {
		string s;
		cin >> s;

		if (s == "exit")
			break;

		tcp_server->PutCmd(unique_ptr<string>(new string(s)));
	}
	tcp_server->Stop();
	WorkerThreadManager::Get()->Stop();
	return 0;
}

