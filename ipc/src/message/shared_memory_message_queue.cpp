#include "shared_memory_message_queue.h"
#include "../crc/crc32.h"
#include "../message/message_object.h"
#include "pch.h"

_IPC_BEGIN

SharedMemoryMessageQueue::SharedMemoryMessageQueue(std::wstring const& name)
    : message_queue_name(name),
      semaphore(name + L"_semaphore"),
      mutex(name + L"_mutex"),
      shared_memory(name + L"_memory") {}

auto SharedMemoryMessageQueue::create() -> HRESULT {
  HRESULT hr = S_OK;
  do {
    hr = shared_memory.create();
    if (FAILED(hr)) {
      break;
    }
    hr = mutex.create();
    if (FAILED(hr)) {
      break;
    }
    hr = semaphore.create();
    if (FAILED(hr)) {
      break;
    }
  } while (false);

  //初始化通信管道内存分配数据
  PipeMemoryHead* const head = (PipeMemoryHead*)shared_memory.ptr();
  memset(head, 0, sizeof(PipeMemoryHead));
  head->version = IPC_VERSION;
  head->messages_num = 0;
  head->start_message_pos = sizeof(PipeMemoryHead);
  head->end_message_pos = sizeof(PipeMemoryHead);
  head->memory_total_length = shared_memory.length() - sizeof(PipeMemoryHead);
  head->memory_remained = shared_memory.length() - sizeof(PipeMemoryHead);
  return hr;
}
auto SharedMemoryMessageQueue::open() -> HRESULT {
  HRESULT hr = S_OK;
  do {
    hr = shared_memory.open();
    if (FAILED(hr)) {
      break;
    }
    hr = mutex.open();
    if (FAILED(hr)) {
      break;
    }
    hr = semaphore.open();
    if (FAILED(hr)) {
      break;
    }
  } while (false);

  return hr;
}

auto SharedMemoryMessageQueue::close() -> HRESULT {
  semaphore.close();
  mutex.close();
  shared_memory.close();
  return S_OK;
}

auto SharedMemoryMessageQueue::recv_msg(
    const uint32_t timeout,
    std::vector<std::vector<uint8_t>>& const buf_list) -> HRESULT {
  HRESULT hr = S_OK;
  do {
    MutexLockGuard l(this->mutex, timeout);
    if (!l.locked()) {
      hr = HRESULT_FROM_WIN32(ERROR_TIMEOUT);
      break;
    }
    PipeMemoryHead* const head = (PipeMemoryHead*)shared_memory.ptr();
    //没有消息
    if (head->messages_num == 0) {
      hr = HRESULT_FROM_WIN32(ERROR_EMPTY);
      break;
    }
    for (; head->messages_num > 0;) {
      PipeMessageHead msg_head;
      // peek msg head information
      this->peek(head, (uint8_t*)&msg_head, sizeof(msg_head), false);
      std::vector<uint8_t> data;
      data.resize(msg_head.body_len);
      this->peek(head, data.data(), (uint32_t)data.size(), true);
      if (msg_head.body_crc != 0 &&
          msg_head.body_crc != crc32(data.data(), data.size(), CRC32_INIT)) {
        // crc check error
        // todo!
        continue;
      }
      buf_list.push_back(std::move(data));
    }

  } while (false);
  return S_OK;
}

auto SharedMemoryMessageQueue::send_msg(const uint8_t* const buf,
                                        uint32_t const len) -> HRESULT {
  HRESULT hr = S_OK;
  do {
    MutexLockGuard l(this->mutex);
    PipeMemoryHead* const head = (PipeMemoryHead*)shared_memory.ptr();
    if (head->memory_remained < (len + sizeof(PipeMessageHead))) {
      hr = HRESULT_FROM_WIN32(ERROR_DS_USER_BUFFER_TO_SMALL);
      break;
    }
    PipeMessageHead msg_head;
    msg_head.magic = MAGIC_HEAD_NUM;
    msg_head.body_crc = crc32(buf, len, CRC32_INIT);
    msg_head.body_len = len;
    // push head first, without increase message number
    this->push(head, (uint8_t*)&msg_head, sizeof(msg_head), false);
    // push body second
    this->push(head, buf, len, true);
  } while (false);

  return hr;
}

auto SharedMemoryMessageQueue::peek(PipeMemoryHead* const ptr,
                                    uint8_t* const out_buf,
                                    uint32_t const len,
                                    bool const dec_message_num) -> void {
  assert(ptr->messages_num > 0, "pipe message is empty");
  if (ptr->start_message_pos + len > ptr->memory_total_length) {
    //循环读取数据
    const auto first_len = ptr->memory_total_length - ptr->start_message_pos;
    memcpy(out_buf, (uint8_t*)ptr + ptr->start_message_pos, first_len);
    const auto second_out_buf = out_buf + first_len;
    const auto second_len = len - first_len;
    memcpy(second_out_buf, (uint8_t*)ptr + sizeof(PipeMemoryHead), second_len);
    ptr->start_message_pos = sizeof(PipeMemoryHead) + second_len;
  } else {
    //直接拷贝消息
    memcpy(out_buf, (uint8_t*)ptr + ptr->start_message_pos, len);
    ptr->start_message_pos = ptr->start_message_pos + len;
  }
  if (dec_message_num) {
    ptr->messages_num -= 1;
  }
  //取完数据，增加空闲空间数量
  ptr->memory_remained += len;
}

auto SharedMemoryMessageQueue::push(PipeMemoryHead* const ptr,
                                    const uint8_t* const buf,
                                    const uint32_t len,
                                    const bool inc_message_num) -> void {
  assert(len <= ptr->memory_remained, "remained buffer too small");
  if (ptr->end_message_pos + len > ptr->memory_total_length) {
    //处理循环情况
    //拷贝第一小段数据到
    uint32_t first_len = ptr->memory_total_length - ptr->end_message_pos;
    memcpy((uint8_t*)ptr + ptr->end_message_pos, buf, first_len);
    const auto second_buf = buf + first_len;
    const uint32_t second_len = len - first_len;
    memcpy((uint8_t*)ptr + sizeof(PipeMemoryHead), second_buf, second_len);
    ptr->end_message_pos = sizeof(PipeMemoryHead) + second_len;
  } else {
    //有足够的空间，直接拷贝数据
    memcpy((uint8_t*)ptr + ptr->end_message_pos, buf, len);
    ptr->end_message_pos += len;
  }

  if (inc_message_num) {
    ptr->messages_num += 1;
  }
  ptr->memory_remained -= len;
}

_IPC_END