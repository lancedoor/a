// t.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <thread>
using namespace std;
#include "../frame/Net/TcpServer.h"

class MyTcpServer : public TcpServer {

};

int main()
{
  MyTcpServer server;
  server.Start();

  for (;;) {
    server.Poll();
    this_thread::sleep_for(chrono::milliseconds(1));
  }

  server.Stop();

    return 0;
}

