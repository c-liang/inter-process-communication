#pragma once
#include "framework.h"
#include "message_queue_traits.h"

_IPC_BEGIN

class SharedMemoryMessageQueue;
class MessagePipe {
public:
	MessagePipe(std::wstring& pipe_name, uint8_t* begin, size_t len);
	auto create()->HRESULT;
	auto open()->HRESULT;
	auto close()->HRESULT;
	auto recv_msg()->void;
	auto send_msg(const uint8_t buf, uint32_t len)->void;
private:

private:
	std::wstring pipe_name;
	std::unique_ptr<MessageQueueTrait> message_queue;
};

_IPC_END