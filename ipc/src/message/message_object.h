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
  u64 fromPeer;
  // reply message from witch
  u64 fromId;
  std::vector<int8_t> data;
};

// pipe messages shared head
struct PipeMemoryHead {
  // version
  u32 version;
  // how many messages in cur pipe
  u32 messages_num;
  // message position starts
  u32 start_message_pos;
  // message position end
  u32 end_message_pos;
  u32 memory_total_length;
  u32 memory_remained;
};
// head added by message pipe
struct PipeMessageHead {
  u32 magic;
  u32 body_crc;
  u32 body_len;
};

#pragma pack(pop)

_IPC_END