#include "../net/TcpClient.h"

struct MP_Packet : public ActorMsgQ::MsgParams {
  shared_ptr<::google::protobuf::Message> packet;
};

class MyTcpClient : public TcpClient {
public:
  MyTcpClient(int32_t associated_actor) {
    associated_actor_ = associated_actor;
  }
  virtual void OnPacket(shared_ptr<::google::protobuf::Message> packet) {
    auto params = make_shared<MP_Packet>();
    params->packet = packet;
    ActorMsgQ::Get()->PostMsg(-2, associated_actor_, -2, params);
  }
private:
  int32_t associated_actor_;
};