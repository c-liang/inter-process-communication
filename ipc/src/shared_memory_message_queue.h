#pragma once
#include "framework.h"
#include "message_queue_traits.h"
#include "message_object.h"
_IPC_BEGIN

class SharedMemoryMessageQueue : public MessageQueueTrait {
public:
	SharedMemoryMessageQueue(const std::wstring& name);
	virtual auto create()->HRESULT override;
	virtual auto open()->HRESULT override;
	virtual auto close()->HRESULT override;
	virtual auto recv_msg(DWORD timeout, std::vector<uint8_t>& buf)->HRESULT override;
	virtual auto send_msg(const uint8_t buf, uint32_t len)->HRESULT override;
private:
	SharedMemoryMessageQueue(const SharedMemoryMessageQueue&) = delete;
	auto operator= (SharedMemoryMessageQueue&& v)->SharedMemoryMessageQueue & = delete;
	auto operator= (const SharedMemoryMessageQueue&)->SharedMemoryMessageQueue & = delete;
private:
	std::wstring shared_memory_name;
	HANDLE shared_memory_semaphore = nullptr;
	HANDLE shared_memory_lock = nullptr;
};

_IPC_END