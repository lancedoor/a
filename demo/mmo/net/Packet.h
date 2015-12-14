#pragma once
#include "google/protobuf/message.h"

void GetPacketBlock(shared_ptr<::google::protobuf::Message> packet, shared_ptr<uint8_t> &ptr, int32_t &size);
