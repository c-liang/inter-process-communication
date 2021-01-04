#include "pch.h"
#include "shared_memory_message_queue.h"

_IPC_BEGIN
SharedMemoryMessageQueue::SharedMemoryMessageQueue(const std::wstring& name) :
	message_queue_name(name),
	semaphore(name + L"_semaphore"),
	mutex(name + L"_mutex"),
	shared_memory(name + L"_memory") {

}

auto SharedMemoryMessageQueue::create() -> HRESULT {
	shared_memory.create();
	mutex.create();
	semaphore.create();
	return S_OK;
}
auto SharedMemoryMessageQueue::open() ->HRESULT {
	shared_memory.open();
	mutex.open();
	semaphore.open();
	return S_OK;
}

auto SharedMemoryMessageQueue::close() ->HRESULT {
	return S_OK;
}

auto SharedMemoryMessageQueue::recv_msg(DWORD timeout, std::vector<std::vector<uint8_t>>& buf_list)->HRESULT {
	return S_OK;
}

auto SharedMemoryMessageQueue::send_msg(const uint8_t buf, uint32_t len) ->HRESULT
{
	return S_OK;
}

_IPC_END