#include "pch.h"
#include "message_pipe.h"
#include "shared_memory_message_queue.h"
_IPC_BEGIN

MessagePipe::MessagePipe(std::wstring const& const pipe_name) {
	this->message_queue = std::make_unique<SharedMemoryMessageQueue>(this->pipe_name);
}

MessagePipe::~MessagePipe() {
	this->closed = true;
	this->close();
	send_cv.notify_all();

	if (this->send_thread.joinable()) {
		this->send_thread.join();
	}
	if (this->recv_thread.joinable()) {
		this->recv_thread.join();
	}
	if (this->heartbeat_thread.joinable()) {
		this->heartbeat_thread.join();
	}
}

auto MessagePipe::create() ->HRESULT {
	return this->message_queue->create();
}

auto MessagePipe::open() ->HRESULT {
	return this->message_queue->open();
}

auto MessagePipe::close() ->HRESULT {
	this->closed = true;
	return this->message_queue->close();
}

auto MessagePipe::recv_msg() ->void {

}

auto MessagePipe::send_msg(const uint8_t* buf, const uint32_t len) ->void {
	this->message_queue->send_msg(buf, len);
}

_IPC_END
