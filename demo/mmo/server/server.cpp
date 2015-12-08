// server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "../net/all.h"
#include "../net/TcpServer.h"
#include "../net/MessageManager.h"
#include "../net/WorkerThread.h"

#pragma comment(lib, "net.lib")

int main()
{
	WorkerThread worker_thread;
	worker_thread.Start();

	TcpServer tcp_server;
	tcp_server.Start();
	for (;;) {
		string s;
		cin >> s;

		if (s == "exit")
			break;

		tcp_server.PutCmd(unique_ptr<string>(new string(s)));
		//MessageManager::Get()->PutMessage(0, 1, 0, s);
	}
	tcp_server.Stop();
	worker_thread.Stop();

	//boost::asio::io_service io_service;

	//TcpAcceptor acceptor(io_service);
	//acceptor.Start();

	//shared_ptr<TcpConnection> conn;
	//for (;;) {
	//	conn->Start();
	//	//string s;
	//	//conn->Recv(s);
	//	//cout << "Client: " << s << endl;
	//	conn->Send("Dismiss. Soldier.");
	//	//conn->Close();
	//}
	//io_service.run();

    return 0;
}

