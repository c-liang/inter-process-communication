#pragma once
#include "../message/message_object.h"
#include "framework.h"
#include "message_object.h"
#include "message_queue_traits.h"
#include "platform/win32/win_mutex.h"
#include "platform/win32/win_semaphore.h"
#include "platform/win32/win_shared_memory.h"
_IPC_BEGIN

class SharedMemoryMessageQueue : public MessageQueueTrait {
 public:
  SharedMemoryMessageQueue(std::wstring const& name);
  ~SharedMemoryMessageQueue();
  virtual auto create() -> HRESULT override;
  virtual auto open() -> HRESULT override;
  virtual auto pre_close() -> HRESULT override;
  virtual auto close() -> HRESULT override;
  virtual auto recv_msg(const u32 timeout,
                        std::vector<std::vector<u8>>& const buf_list)
      -> HRESULT override;
  virtual auto send_msg(const u8* const buf, u32 const len) -> HRESULT override;

 private:
  auto peek(PipeMemoryHead* const ptr,
            u8* const out_buf,
            u32 const len,
            bool const dec_message_num) -> void;
  auto push(PipeMemoryHead* const ptr,
            const u8* const buf,
            u32 const len,
            const bool inc_message_num) -> void;

 private:
  SharedMemoryMessageQueue(const SharedMemoryMessageQueue&) = delete;
  auto operator=(SharedMemoryMessageQueue&& v)
      -> SharedMemoryMessageQueue& = delete;
  auto operator=(const SharedMemoryMessageQueue&)
      -> SharedMemoryMessageQueue& = delete;

 private:
  std::wstring message_queue_name;
  Semaphore semaphore;
  Mutex mutex;
  SharedMemory shared_memory;
};

_IPC_END