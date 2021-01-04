#pragma once
#include "framework.h"
#include "message_queue_traits.h"
#include "message_object.h"
#include "platform/win32/win_mutex.h"
#include "platform/win32/win_semaphore.h"
#include "platform/win32/win_shared_memory.h"
#include "../message/message_object.h"
_IPC_BEGIN

class SharedMemoryMessageQueue : public MessageQueueTrait {
public:
	SharedMemoryMessageQueue(const std::wstring& name);
	virtual auto create()->HRESULT override;
	virtual auto open()->HRESULT override;
	virtual auto close()->HRESULT override;
	virtual auto recv_msg(const uint32_t timeout, std::vector<std::vector<uint8_t>>& buf_list)->HRESULT override;
	virtual auto send_msg(const uint8_t* buf, const uint32_t len)->HRESULT override;
private:
	auto peek(
		PipeMemoryHead* ptr,
		uint8_t* out_buf,
		const uint32_t len,
		const bool dec_message_num
	)->void;
	auto push(
		PipeMemoryHead* ptr,
		const uint8_t* buf,
		const uint32_t len,
		const bool inc_message_num
	)->void;
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