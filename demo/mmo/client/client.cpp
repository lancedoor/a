// client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "../net/all.h"
#include "../net/TcpClient.h"
#include "../net/MessageManager.h"
#include <boost/archive/binary_oarchive.hpp>

#pragma comment(lib, "net.lib")

int main()
{
	TcpClient tcp_client;
	tcp_client.Start();

	//boost::asio::io_service io_service;

	//auto conn = TcpConnector::Connect(io_service);
	//if (conn) {
	//	conn->Start();
	//	conn->Send("Good morning. Commandor.");
	//	//string s;
	//	//conn->Recv(s);
	//	//cout << "Server: " << s << endl;
	//	//conn->Close();
	//}
	//io_service.run();

	for (;;) {
		string s;
		cin >> s;
		//cout << s << endl;

		if (s == "exit")
			break;

		tcp_client.PutCmd(unique_ptr<string>(new string(s)));
		//MessageManager::Get()->PutMessage(0, 1, 0, s);
	}
	tcp_client.Stop();
    return 0;
}

