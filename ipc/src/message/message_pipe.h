#pragma once
#include "../framework.h"
#include "message_queue_traits.h"

_IPC_BEGIN

class MessagePipe {
 public:
  class RecvCallback {
   public:
    virtual ~RecvCallback() {}
    virtual void received(u8* data, uint32_t len){};
  };

 public:
  MessagePipe(std::wstring const& const pipe_name);
  ~MessagePipe();
  // only receive message when creates
  auto create(RecvCallback* callback) -> HRESULT;
  auto open() -> HRESULT;
  auto close() -> HRESULT;
  // open pipe and send message ,who creates this pipe will receive this message
  auto send_msg(const uint8_t* buf, const uint32_t len) -> void;

 private:
  auto send_loop() -> void;
  auto recv_loop() -> void;
#ifdef _IPC_MSG_HEANLD_THREAD
  auto handle_msg_loop() -> void;
#endif
 private:
  std::atomic<bool> closed = false;
  std::atomic<RecvCallback*> recv_callback = nullptr;
  // handle msg
  std::thread handle_msg_thread;
  std::condition_variable recv_msg_cv;
  std::mutex recv_msg_mutex;
  std::vector<std::vector<u8>> recv_msg_queue;
  std::wstring pipe_name;
  std::thread recv_thread;
  std::thread send_thread;
  std::condition_variable send_cv;
  std::mutex send_mutex;
  std::vector<std::vector<u8>> send_queue;
  std::vector<std::vector<u8>> pending_message;
  std::unique_ptr<MessageQueueTrait> message_queue;
};

_IPC_END