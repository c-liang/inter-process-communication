#pragma once
#include "framework.h"
#include "message_queue_traits.h"
#include "message_object.h"
#include "platform/win32/win_mutex.h"
#include "platform/win32/win_semaphore.h"
#include "platform/win32/win_shared_memory.h"
_IPC_BEGIN

class SharedMemoryMessageQueue : public MessageQueueTrait {
public:
	SharedMemoryMessageQueue(const std::wstring& name);
	virtual auto create()->HRESULT override;
	virtual auto open()->HRESULT override;
	virtual auto close()->HRESULT override;
	virtual auto recv_msg(DWORD timeout, std::vector<std::vector<uint8_t>>& buf_list)->HRESULT override;
	virtual auto send_msg(const uint8_t buf, uint32_t len)->HRESULT override;
private:
	SharedMemoryMessageQueue(const SharedMemoryMessageQueue&) = delete;
	auto operator= (SharedMemoryMessageQueue&& v)->SharedMemoryMessageQueue & = delete;
	auto operator= (const SharedMemoryMessageQueue&)->SharedMemoryMessageQueue & = delete;
private:
	std::wstring message_queue_name;
	Semaphore semaphore;
	Mutex mutex;
	SharedMemory shared_memory;
};

_IPC_END