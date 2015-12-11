#include "Packet.h"

DefinePacket(CS_LOGIN, CS_Login)

void GetPacketBlock(int32_t packet_id, shared_ptr<Packet> packet, shared_ptr<uint8_t> &ptr, int32_t &size) {
  size = packet->Bytes() + 4;
  ptr = shared_ptr<uint8_t>(new uint8_t[size]);
  *(int32_t*)ptr.get() = packet_id;
  packet->SerializeToArray(ptr.get() + 4, size - 4);
}

