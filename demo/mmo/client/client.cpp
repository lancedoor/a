// client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "../frame/NetActor/ClientNetActor.h"
#include "../frame/Frame/ClientFrame.h"
#include "../common/Packets.pb.h"
#include "MyClientNetActor.h"
#include "UserInfo.h"

#pragma comment(lib, "libprotobuf.lib")
#pragma comment(lib, "frame.lib")
#pragma comment(lib, "common.lib")

int main()
{
  ClientFrame::Get()->Init(1, make_shared<MyClientNetActor>());
  ClientFrame::Get()->Start();

  for (;;) {
    //cout << UserInfo::Get()->GetName() << ":";
    char c[256];
    cin.getline(c, 256);
		string s(c);

		if (s == "exit")
			break;

    auto packet = make_shared<Packet::CS_Say>();
    packet->set_text(s);
    ClientFrame::Get()->SendPacket(packet);
	}

  ClientFrame::Get()->Stop();
  return 0;
}

