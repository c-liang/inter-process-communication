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

SharedMemoryMessageQueue::~SharedMemoryMessageQueue() {
  this->close();
}

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

  //��ʼ��ͨ�Źܵ��ڴ��������
  PipeMemoryHead* const head = (PipeMemoryHead*)shared_memory.ptr();
  memset(head, 0, sizeof(PipeMemoryHead));
  head->version = IPC_VERSION;
  head->messages_num = 0;
  head->start_message_pos = sizeof(PipeMemoryHead);
  head->end_message_pos = sizeof(PipeMemoryHead);
  head->memory_total_length = shared_memory.length();
  //ʣ�������ݵĿռ䣬������ͷ���ռ�
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

auto SharedMemoryMessageQueue::pre_close() -> HRESULT {
  semaphore.release();
  return S_OK;
}

auto SharedMemoryMessageQueue::close() -> HRESULT {
  semaphore.close();
  mutex.close();
  shared_memory.close();
  return S_OK;
}

auto SharedMemoryMessageQueue::recv_msg(
    const u32 timeout,
    std::vector<std::vector<u8>>& const buf_list) -> HRESULT {
  HRESULT hr = S_OK;
  if (FAILED(this->semaphore.wait_timeout(timeout))) {
    return HRESULT_FROM_WIN32(ERROR_TIMEOUT);
  }

  do {
    MutexLockGuard l(this->mutex, timeout);
    if (!l.locked()) {
      hr = HRESULT_FROM_WIN32(ERROR_TIMEOUT);
      break;
    }
    PipeMemoryHead* const head = (PipeMemoryHead*)shared_memory.ptr();
    //û����Ϣ
    if (head->messages_num == 0) {
      hr = HRESULT_FROM_WIN32(ERROR_EMPTY);
      break;
    }
    int32_t message_nums = head->messages_num;
    for (auto i = 0; i < message_nums; ++i) {
      PipeMessageHead msg_head;
      // peek msg head information
      this->peek(head, (u8*)&msg_head, sizeof(msg_head), false);
      // body content
      std::vector<u8> data;
      data.resize(msg_head.body_len);
      this->peek(head, data.data(), (u32)data.size(), true);

      if (msg_head.magic != MAGIC_HEAD_NUM) {
        // todo!
#ifdef _DEBUG
        DebugBreak();
#endif
        continue;
      }
#ifdef _IPC_CHECK_CRC
      if (msg_head.body_crc != crc32(data.data(), data.size(), CRC32_INIT)) {
// crc check error
// todo!
#ifdef _DEBUG
        DebugBreak();
#endif
        continue;
      }
#endif
      buf_list.push_back(std::move(data));
    }

  } while (false);
  return S_OK;
}

auto SharedMemoryMessageQueue::send_msg(const u8* const buf, u32 const len)
    -> HRESULT {
  HRESULT hr = S_OK;
  do {
    {
      MutexLockGuard l(this->mutex);
      if (!l.locked()) {
        hr = HRESULT_FROM_WIN32(ERROR_LOCK_FAILED);
        break;
      }
      PipeMemoryHead* const head = (PipeMemoryHead*)shared_memory.ptr();
      if (head->memory_remained < (len + sizeof(PipeMessageHead))) {
        hr = HRESULT_FROM_WIN32(ERROR_DS_USER_BUFFER_TO_SMALL);
        break;
      }
      PipeMessageHead msg_head;
      msg_head.magic = MAGIC_HEAD_NUM;
#ifdef _IPC_CHECK_CRC
      msg_head.body_crc = crc32(buf, len, CRC32_INIT);
#else
      msg_head.body_crc = 0;
#endif
      msg_head.body_len = len;
      // push head first, without increase message number
      this->push(head, (uint8_t*)&msg_head, sizeof(msg_head), false);
      // push body second
      this->push(head, buf, len, true);
    }
    this->semaphore.release();
  } while (false);

  return hr;
}

auto SharedMemoryMessageQueue::peek(PipeMemoryHead* const ptr,
                                    u8* const out_buf,
                                    u32 const len,
                                    bool const dec_message_num) -> void {
  assert(ptr->messages_num > 0, "pipe message is empty");
  if (ptr->start_message_pos + len > ptr->memory_total_length) {
    //ѭ����ȡ����
    const auto first_len = ptr->memory_total_length - ptr->start_message_pos;
    memcpy(out_buf, (uint8_t*)ptr + ptr->start_message_pos, first_len);
    const auto second_out_buf = out_buf + first_len;
    const auto second_len = len - first_len;
    memcpy(second_out_buf, (uint8_t*)ptr + sizeof(PipeMemoryHead), second_len);
    ptr->start_message_pos = sizeof(PipeMemoryHead) + second_len;
  } else {
    //ֱ�ӿ�����Ϣ
    memcpy(out_buf, (uint8_t*)ptr + ptr->start_message_pos, len);
    ptr->start_message_pos = ptr->start_message_pos + len;
  }
  if (dec_message_num) {
    ptr->messages_num -= 1;
  }
  //ȡ�����ݣ����ӿ��пռ�����
  ptr->memory_remained += len;
}

auto SharedMemoryMessageQueue::push(PipeMemoryHead* const ptr,
                                    const u8* const buf,
                                    const u32 len,
                                    const bool inc_message_num) -> void {
  assert(len <= ptr->memory_remained, "remained buffer too small");
  if (ptr->end_message_pos + len > ptr->memory_total_length) {
    //����ѭ�����
    //������һС�����ݵ�
    u32 first_len = ptr->memory_total_length - ptr->end_message_pos;
    memcpy((u8*)ptr + ptr->end_message_pos, buf, first_len);
    const auto second_buf = buf + first_len;
    const u32 second_len = len - first_len;
    memcpy((u8*)ptr + sizeof(PipeMemoryHead), second_buf, second_len);
    ptr->end_message_pos = sizeof(PipeMemoryHead) + second_len;
  } else {
    //���㹻�Ŀռ䣬ֱ�ӿ�������
    memcpy((u8*)ptr + ptr->end_message_pos, buf, len);
    ptr->end_message_pos += len;
  }

  if (inc_message_num) {
    ptr->messages_num += 1;
  }
  ptr->memory_remained -= len;
}

_IPC_END