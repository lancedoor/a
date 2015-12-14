#pragma once

enum PacketId {
  CS_LOGIN,
  CS_CHAT,
};

class Packet {
public:
  virtual size_t ByteSize() { return 0; }
  virtual void SerializeFromArray(uint8_t *ptr, uint32_t size) {}
  virtual void SerializeToArray(uint8_t *ptr, uint32_t size) {}
};

class CS_Login : public Packet{
public:
  string user;
  virtual size_t ByteSize() { return user.size(); }
  void SerializeFromArray(uint8_t *ptr, uint32_t size) {
    user = string((const char*)ptr, size);
  }
  virtual void SerializeToArray(uint8_t *ptr, uint32_t size) {
    if (size < user.size())
      return;
    memcpy(ptr, user.data(), user.size());
  }
};

class CS_Chat : public Packet {
public:
  string text;
  virtual size_t ByteSize() { return text.size(); }
  void SerializeFromArray(uint8_t *ptr, uint32_t size) {
    text = string((const char*)ptr, size);
  }
  virtual void SerializeToArray(uint8_t *ptr, uint32_t size) {
    if (size < text.size())
      return;
    memcpy(ptr, text.data(), text.size());
  }
};

//todo: optimize
//shared_ptr<Packet> CreatePacket(int32_t packet_id) {
//  switch (packet_id) {
//  case PacketId::CS_LOGIN: return make_shared<CS_Login>();
//  case PacketId::CS_CHAT: return make_shared<CS_Chat>();
//  }
//  return nullptr;
//}

//#define DefinePacket(ID, TYPE)\
//int GetPacketId(shared_ptr<TYPE>) {return ID;}

//void GetPacketBlock(int32_t packet_id, shared_ptr<Packet> packet, shared_ptr<uint8_t> &ptr, int32_t &size);
void GetPacketBlock(shared_ptr<Packet> packet, shared_ptr<uint8_t> &ptr, int32_t &size);
