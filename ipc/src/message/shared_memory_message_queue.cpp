#include "pch.h"
#include "../crc/crc32.h"
#include "../message/message_object.h"
#include "shared_memory_message_queue.h"

_IPC_BEGIN
SharedMemoryMessageQueue::SharedMemoryMessageQueue(std::wstring const& name) :
	message_queue_name(name),
	semaphore(name + L"_semaphore"),
	mutex(name + L"_mutex"),
	shared_memory(name + L"_memory") {

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

	//初始化通信管道内存分配数据
	PipeMemoryHead* head = (PipeMemoryHead*)shared_memory.ptr();
	memset(head, 0, sizeof(PipeMemoryHead));
	head->messages_num = 0;
	head->start_message_pos = sizeof(PipeMemoryHead);
	head->end_message_pos = sizeof(PipeMemoryHead);
	head->memory_total_length = shared_memory.length() - sizeof(PipeMemoryHead);
	head->memory_remained = shared_memory.length() - sizeof(PipeMemoryHead);
	return hr;
}
auto SharedMemoryMessageQueue::open() ->HRESULT {
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

auto SharedMemoryMessageQueue::close() ->HRESULT {
	return S_OK;
}

auto SharedMemoryMessageQueue::recv_msg(
	const uint32_t timeout,
	std::vector<std::vector<uint8_t>>& buf_list
)->HRESULT {

	return S_OK;
}

auto SharedMemoryMessageQueue::send_msg(
	const uint8_t* buf,
	const uint32_t len
) ->HRESULT {

	HRESULT hr = S_OK;
	do {
		MutexLockGuard l(this->mutex);
		PipeMemoryHead* const head = (PipeMemoryHead*)shared_memory.ptr();
		if (head->memory_remained < (len + sizeof(PipeMessageHead))) {
			hr = ERROR_DS_USER_BUFFER_TO_SMALL;
			break;
		}
		PipeMessageHead msg_head;
		msg_head.magic = MAGIC_HEAD_NUM;
		msg_head.body_crc = crc32(buf, len, CRC32_INIT);
		msg_head.body_len = len;
		//push head first, without increase message number
		this->push(head, (uint8_t*)&msg_head, sizeof(msg_head), false);
		//push body second
		this->push(head, buf, len, true);
	} while (false);

	return hr;
}

auto SharedMemoryMessageQueue::peek(
	PipeMemoryHead* const ptr,
	uint8_t* const out_buf,
	uint32_t const len,
	bool const dec_message_num
)->void {
	assert(ptr->messages_num, "pipe message is empty");
	if (dec_message_num) {
		ptr->messages_num -= 1;
	}
	ptr->memory_remained += sizeof(PipeMessageHead) + len;
}

auto SharedMemoryMessageQueue::push(
	PipeMemoryHead* const ptr,
	const uint8_t* const buf,
	const uint32_t len,
	const bool inc_message_num
) -> void {
	assert(len <= ptr->memory_remained, "remained buffer too small");
	if (ptr->end_message_pos + len > ptr->memory_total_length) {
		//处理循环情况
		uint32_t first_len = ptr->memory_total_length - ptr->end_message_pos;
		memcpy((uint8_t*)ptr + ptr->end_message_pos, buf, first_len);
		const auto second_buf = buf + first_len;
		const uint32_t second_len = len - first_len;
		memcpy((uint8_t*)ptr + sizeof(PipeMemoryHead), second_buf, second_len);
		ptr->end_message_pos = sizeof(PipeMemoryHead) + second_len;
	}
	else {
		memcpy((uint8_t*)ptr + ptr->end_message_pos, buf, len);
		ptr->end_message_pos += len;
	}

	if (inc_message_num) {
		ptr->messages_num += 1;
	}
	ptr->memory_remained -= len;
}

_IPC_END