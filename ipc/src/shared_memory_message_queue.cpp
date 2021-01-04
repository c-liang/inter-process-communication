#include "pch.h"
#include "shared_memory_message_queue.h"

_IPC_BEGIN
SharedMemoryMessageQueue::SharedMemoryMessageQueue(const std::wstring& name) :
	shared_memory_name(name) {

}

auto SharedMemoryMessageQueue::create() -> HRESULT {
	return S_OK;
}
auto SharedMemoryMessageQueue::open() ->HRESULT {
	return S_OK;
}

auto SharedMemoryMessageQueue::close() ->HRESULT {
	return S_OK;
}

auto SharedMemoryMessageQueue::recv_msg(DWORD timeout, std::vector<uint8_t>& buf)->HRESULT {
	return S_OK;
}

auto SharedMemoryMessageQueue::send_msg(const uint8_t buf, uint32_t len) ->HRESULT
{
	return S_OK;
}




_IPC_END