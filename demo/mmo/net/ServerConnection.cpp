#include "ServerConnection.h"
#include "TcpServer.h"

void ServerConnection::OnRecvPacket(shared_ptr<::google::protobuf::Message> packet)
{
  auto p = tcp_server_.lock();
  if (p)
    p->OnSessionPacket(session_id_, packet);
}
