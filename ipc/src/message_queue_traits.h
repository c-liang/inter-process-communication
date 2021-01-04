#pragma once
#include "framework.h"
_IPC_BEGIN

class MessageQueueTrait {
public:
	virtual ~MessageQueueTrait() = 0;
	virtual auto create()->HRESULT = 0;
	virtual auto open()->HRESULT = 0;
	virtual auto close()->HRESULT = 0;
	virtual auto recv_msg(DWORD timeout, std::vector<uint8_t>& buf)->HRESULT = 0;
	virtual auto send_msg(const uint8_t buf, uint32_t len)->HRESULT = 0;
};

_IPC_END