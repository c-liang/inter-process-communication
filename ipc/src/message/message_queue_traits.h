#pragma once
#include "framework.h"
_IPC_BEGIN

class MessageQueueTrait {
public:
	virtual ~MessageQueueTrait() = 0;
	virtual auto create()->HRESULT = 0;
	virtual auto open()->HRESULT = 0;
	virtual auto close()->HRESULT = 0;
	virtual auto recv_msg(const DWORD timeout, std::vector<std::vector<uint8_t>>& buf_list)->HRESULT = 0;
	virtual auto send_msg(const uint8_t* buf, const uint32_t len)->HRESULT = 0;
};

_IPC_END