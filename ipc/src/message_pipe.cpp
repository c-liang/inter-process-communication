#include "pch.h"
#include "message_pipe.h"
#include "shared_memory_message_queue.h"
_IPC_BEGIN

MessagePipe::MessagePipe(std::wstring& pipe_name, uint8_t* begin, size_t len) {
	this->message_queue = std::make_unique<SharedMemoryMessageQueue>(this->pipe_name);
}

auto MessagePipe::create() ->HRESULT {
	return this->message_queue->create();
}

auto MessagePipe::open() ->HRESULT {
	return this->message_queue->open();
}

auto MessagePipe::close() ->HRESULT {
	this->message_queue->close();
}

auto MessagePipe::recv_msg() ->void {

}

auto MessagePipe::send_msg(const uint8_t buf, uint32_t len) ->void {
	this->message_queue->send_msg(buf, len);
}

_IPC_END
