#include "message_pipe.h"

#include "pch.h"
#include "shared_memory_message_queue.h"
_IPC_BEGIN

MessagePipe::MessagePipe(std::wstring const& const pipe_name)
    : pipe_name(pipe_name) {
  this->message_queue =
      std::make_unique<SharedMemoryMessageQueue>(this->pipe_name);
}

MessagePipe::~MessagePipe() {
  this->close();
}

auto MessagePipe::create(RecvCallback* callback) -> HRESULT {
  HRESULT hr = this->message_queue->create();
  if (FAILED(hr)) {
    return hr;
  }
  this->recv_callback.store(callback);
  this->recv_thread = std::thread(&MessagePipe::recv_loop, this);
  return hr;
}

auto MessagePipe::open() -> HRESULT {
  HRESULT hr = this->message_queue->open();
  if (FAILED(hr)) {
    return hr;
  }
  this->send_thread = std::thread(&MessagePipe::send_loop, this);
  return hr;
}

auto MessagePipe::close() -> HRESULT {
  this->closed = true;
  send_cv.notify_all();
  this->message_queue->pre_close();
  if (this->send_thread.joinable()) {
    this->send_thread.join();
  }
  if (this->recv_thread.joinable()) {
    this->recv_thread.join();
  }
  return this->message_queue->close();
}

auto MessagePipe::send_msg(const uint8_t* buf, const uint32_t len) -> void {
  {
    std::vector<uint8_t> data;
    data.resize(len);
    memcpy(data.data(), buf, data.size());
    std::lock_guard<std::mutex> l(this->send_mutex);
    this->send_queue.push_back(std::move(data));
  }
  this->send_cv.notify_all();
}

auto MessagePipe::send_loop() -> void {
  for (;;) {
    if (this->closed) {
      //最后发送一次
      std::lock_guard<std::mutex> l(this->send_mutex);
      for (auto&& v : this->send_queue) {
        this->message_queue->send_msg(v.data(), v.size());
      }
      this->send_queue.clear();
      break;
    }
    std::vector<std::vector<uint8_t>> to_send;
    {
      std::unique_lock<std::mutex> l(this->send_mutex);
      send_cv.wait(l, [this] {
        return this->closed || !this->send_queue.empty() ||
               !this->pending_message.empty();
      });

      to_send.swap(pending_message);
      for (auto&& v : this->send_queue) {
        to_send.push_back(std::move(v));
      }
      this->send_queue.clear();
    }
    for (auto&& v : to_send) {
      //发送失败加入错误队列
      if (FAILED(this->message_queue->send_msg(v.data(), v.size()))) {
        this->pending_message.push_back(std::move(v));
      }
    }
  }
}

auto MessagePipe::recv_loop() -> void {
  for (;;) {
    std::vector<std::vector<uint8_t>> msgs;
    this->message_queue->recv_msg(10 * 1000, msgs);
    if (this->closed) {
      break;
    }
    auto callback = recv_callback.load();
    if (callback) {
      for (auto& msg : msgs) {
        callback->received(msg.data(), msg.size());
      }
    }
  }
}

auto MessagePipe::heartbeat_loop() -> void {}

_IPC_END
