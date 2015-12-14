#pragma once
#include "Packet.h"
#include "google/protobuf/message.h"

class PacketType {
public:
  PacketType() {
  }
  PacketType(shared_ptr<::google::protobuf::Message> packet) {
    type_name_ = packet->GetTypeName();
  }
  bool operator== (const PacketType &rhs) const {
    return type_name_ == rhs.type_name_;
  }

  uint32_t ByteSize() {
    return sizeof(uint32_t) + type_name_.size();
  }
  bool SerializeTo(uint8_t* ptr, uint32_t size) {
    if (size < ByteSize())
      return false;

    *((uint32_t*)ptr) = type_name_.size();
    memcpy(ptr + sizeof(uint32_t), type_name_.data(), type_name_.size());
    return true;
  }
  uint32_t SerializeFrom(const uint8_t* ptr, uint32_t size) {
    if (size < sizeof(uint32_t))
      return 0;

    uint32_t len = *((uint32_t*)ptr);
    if (size < sizeof(uint32_t) + len)
      return 0;

    type_name_ = string((const char *)ptr + sizeof(uint32_t), len);

    return sizeof(uint32_t) + len;
  }

  shared_ptr<::google::protobuf::Message> CreatePacket() {
    auto d = ::google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(type_name_);
    if (!d)
      return nullptr;
    auto t = ::google::protobuf::MessageFactory::generated_factory()->GetPrototype(d);
    if (!t)
      return nullptr;
    return shared_ptr<::google::protobuf::Message>(t->New());
  }
//private:
  string type_name_;
};

template<>
struct hash<PacketType> {
  size_t operator ()(const PacketType &packet_type) const {
    hash<string> h;
    return h(packet_type.type_name_);
  }
};