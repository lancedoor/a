// client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "MyTcpClient.h"
#include "../common/Packets.pb.h"
#include "UserInfo.h"

#pragma comment(lib, "net.lib")
#pragma comment(lib, "common.lib")
#pragma comment(lib, "libprotobuf.lib")

int main()
{
	auto tcp_client = make_shared<MyTcpClient>();
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

