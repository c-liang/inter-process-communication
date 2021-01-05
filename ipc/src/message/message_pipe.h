#pragma once
#include "../framework.h"
#include "message_queue_traits.h"

_IPC_BEGIN

class SharedMemoryMessageQueue;
class MessagePipe {
 public:
  MessagePipe(std::wstring const& const pipe_name);
  ~MessagePipe();
  auto create() -> HRESULT;
  auto open() -> HRESULT;
  auto close() -> HRESULT;
  auto recv_msg() -> void;
  auto send_msg(const uint8_t* buf, const uint32_t len) -> void;

 private:
  auto send_loop() -> void;
  auto recv_loop() -> void;
  auto heartbeat_loop() -> void;

 private:
  std::atomic<bool> closed = false;
  std::wstring pipe_name;
  std::thread heartbeat_thread;
  std::thread recv_thread;
  std::thread send_thread;
  std::condition_variable send_cv;
  std::mutex send_mutex;
  std::vector<std::vector<uint8_t>> send_queue;
  std::vector<std::vector<uint8_t>> pending_message;
  std::unique_ptr<MessageQueueTrait> message_queue;
};

_IPC_END