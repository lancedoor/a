#include "Packet.h"
#include "PacketType.h"

void GetPacketBlock(shared_ptr<::google::protobuf::Message> packet, shared_ptr<uint8_t> &ptr, int32_t &size)
{
  PacketType packet_type(packet);

  auto packet_type_size = packet_type.ByteSize();
  size = packet_type_size + packet->ByteSize();

  ptr = shared_ptr<uint8_t>(new uint8_t[size]);
  packet_type.SerializeTo(ptr.get(), packet_type_size);
  packet->SerializeToArray(ptr.get() + packet_type_size, size - packet_type_size);
}

