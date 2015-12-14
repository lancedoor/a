#include "ClientConnection.h"
#include "TcpClient.h"

void ClientConnection::OnRecvPacket(shared_ptr<::google::protobuf::Message> packet)
{
  auto p = tcp_client_.lock();
  if (p)
    p->OnPacket(packet);
}
