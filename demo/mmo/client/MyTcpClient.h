#include "../net/TcpClient.h"
#include "../common/Packets.pb.h"

class MyTcpClient : public TcpClient {
public:
  virtual void OnPacket(shared_ptr<::google::protobuf::Message> packet) {
    PacketType packet_type(packet);
    if (packet_type == PacketType(make_shared<Packet::SC_LoginResult>())) {
      auto p = dynamic_pointer_cast<Packet::SC_LoginResult>(packet);
      cout << p->welcome() << " " << p->name() << endl;
    }
    else if (packet_type == PacketType(make_shared<Packet::SC_SomeoneSay>())) {
      auto p = dynamic_pointer_cast<Packet::SC_SomeoneSay>(packet);
      cout << p->name() << ": " << p->text() << endl;
    }
  }
};