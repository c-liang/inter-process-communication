#pragma once
#include "framework.h"

_IPC_BEGIN

static constexpr int32_t MAGIC_HEAD_NUM = 0x5522365;
enum MessageType {
  // msg client to server when a client try to connect server
  ConnectToServer,
  //
  DisconnectFromServer,
  ServerClosed,
  Heartbeat,
  //
  Communication,
  CommunicationReply
};
#pragma pack(push, _IPC_MESSAGE_PACKING)
struct MessageHead {
  int32_t magic_head_val;
  MessageType type;
  int64_t id;
};

struct MessageRecv {
  int64_t id;
  std::vector<int8_t> data;

  bool withReply;
  // only used if message need reply
  std::vector<int8_t> replyData;
};

struct MessageSend {
  bool withReply;
  std::vector<int8_t> data;
};

struct MessageReply {
  // which peer replied this message
  int64_t fromPeer;
  // reply message from witch
  int64_t fromId;
  std::vector<int8_t> data;
};

// pipe messages shared head
struct PipeMemoryHead {
  // version
  uint32_t version;
  // how many messages in cur pipe
  int32_t messages_num;
  // message position starts
  uint32_t start_message_pos;
  // message position end
  uint32_t end_message_pos;
  uint32_t memory_total_length;
  uint32_t memory_remained;
};
// head added by message pipe
struct PipeMessageHead {
  uint32_t magic;
  uint32_t body_crc;
  uint32_t body_len;
};

#pragma pack(pop)

_IPC_END