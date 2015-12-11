#pragma once
#include "Packet.h"

class PacketType {
public:
  PacketType() {
    type_ = -1;
  }
  bool IsInited() {
    return type_ != -1;
  }
  uint32_t ByteSize() {
    return 4;
  }
  bool SerializeTo(uint8_t* ptr, uint32_t size) {
    if (!IsInited())
      return false;
    if (size < ByteSize())
      return false;

    *((int32_t*)ptr) = type_;
    return true;
  }
  uint32_t SerializeFrom(const uint8_t* ptr, uint32_t size) {
    if (size < 4)
      return 0;

    type_ = *((int32_t*)ptr);
    return 4;
  }

  void FromPacket(shared_ptr<Packet> packet) {
    if (typeid(*packet.get()) == typeid(CS_Login))
      type_ = 0;
    else if (typeid(*packet.get()) == typeid(CS_Chat))
      type_ = 1;
  }
  shared_ptr<Packet> CreatePacket() {
    switch (type_) {
    case 0: return make_shared<CS_Login>();
    case 1: return make_shared<CS_Chat>();
    }
    return nullptr;
  }
//private:
  int32_t type_;
};