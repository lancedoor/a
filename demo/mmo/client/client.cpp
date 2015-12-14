// client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "../net/all.h"
#include "../net/TcpClient.h"
#include "../net/MessageManager.h"
#include <boost/archive/binary_oarchive.hpp>
#include "../common/Packets.pb.h"

#pragma comment(lib, "net.lib")
#pragma comment(lib, "common.lib")
#pragma comment(lib, "libprotobuf.lib")

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

  auto packet = make_shared<Packet::CS_Login>();
  packet->set_user("guest");
  tcp_client.SendPacket(packet);

  for (;;) {
		string s;
		cin >> s;
		//cout << s << endl;

		if (s == "exit")
			break;

    auto packet = make_shared<Packet::CS_Say>();
    packet->set_text(s);
    tcp_client.SendPacket(packet);


		//tcp_client.PutCmd(unique_ptr<string>(new string(s)));
		//MessageManager::Get()->PutMessage(0, 1, 0, s);
	}
	tcp_client.Stop();
    return 0;
}

