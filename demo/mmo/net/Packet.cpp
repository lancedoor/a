#include "Packet.h"
#include "PacketType.h"

DefinePacket(CS_LOGIN, CS_Login)

void GetPacketBlock(int32_t packet_id, shared_ptr<Packet> packet, shared_ptr<uint8_t> &ptr, int32_t &size) {
  size = packet->ByteSize() + 4;
  ptr = shared_ptr<uint8_t>(new uint8_t[size]);
  *(int32_t*)ptr.get() = packet_id;
  packet->SerializeToArray(ptr.get() + 4, size - 4);
}

void GetPacketBlock(shared_ptr<Packet> packet, shared_ptr<uint8_t> &ptr, int32_t &size)
{
  PacketType packet_type;
  packet_type.FromPacket(packet);

  auto packet_type_size = packet_type.ByteSize();
  size = packet_type_size + packet->ByteSize();

  ptr = shared_ptr<uint8_t>(new uint8_t[size]);
  packet_type.SerializeTo(ptr.get(), packet_type_size);
  packet->SerializeToArray(ptr.get() + packet_type_size, size - packet_type_size);

}

