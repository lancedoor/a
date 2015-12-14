#pragma once
#include "TcpConnection.h"
#include "PacketType.h"

class TcpPacketConnection : public TcpConnection
{
public:
  TcpPacketConnection(shared_ptr<tcp::socket> _socket)
    : TcpConnection(_socket) {

  }

  void SendPacket(shared_ptr<::google::protobuf::Message> packet) {
    PacketType packet_type(packet);

    auto packet_type_size = packet_type.ByteSize();
    uint32_t block_size = packet_type_size + packet->ByteSize();

    auto block_ptr = shared_ptr<uint8_t>(new uint8_t[block_size]);
    packet_type.SerializeTo(block_ptr.get(), packet_type_size);
    packet->SerializeToArray(block_ptr.get() + packet_type_size, block_size - packet_type_size);

    Send(block_ptr, block_size);
  }
protected:
  virtual void OnRecvPacket(shared_ptr<::google::protobuf::Message> packet) {
  }
private:
  virtual void OnRecv(uint8_t* ptr, uint32_t size) {
    PacketType packet_type;
    auto packet_type_size = packet_type.SerializeFrom(ptr, size);
    if (packet_type_size == 0)
      return;

    auto packet = packet_type.CreatePacket();
    if (!packet)
      return;

    packet->ParseFromArray(ptr + packet_type_size, size - packet_type_size);
    OnRecvPacket(packet);
  }

};

