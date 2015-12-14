#pragma once
#include "TcpPacketConnection.h"

class TcpServer;
class ServerConnection : public TcpPacketConnection
{
public:
  ServerConnection(weak_ptr<TcpServer> tcp_server, shared_ptr<tcp::socket> _socket)
  : TcpPacketConnection(_socket) {
    tcp_server_ = tcp_server;
  }
  void SetSessionId(int32_t id) {
    session_id_ = id;
  }
private:
  virtual void OnRecvPacket(shared_ptr<::google::protobuf::Message> packet);
private:
  weak_ptr<TcpServer> tcp_server_;
  int32_t session_id_;
};

