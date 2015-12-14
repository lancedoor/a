#pragma once
#include "TcpPacketConnection.h"

class TcpClient;
class ClientConnection : public TcpPacketConnection
{
public:
  ClientConnection(weak_ptr<TcpClient> tcp_client, shared_ptr<tcp::socket> _socket)
  : TcpPacketConnection(_socket) {
    tcp_client_ = tcp_client;
  }
private:
  virtual void OnRecvPacket(shared_ptr<::google::protobuf::Message> packet);
private:
  weak_ptr<TcpClient> tcp_client_;
};

