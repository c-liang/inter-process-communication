#pragma once
#include "../../framework.h"
_IPC_BEGIN

class SharedMemory {
public:
	SharedMemory(std::wstring const& const name, uint32_t len = _IPC_SHARED_MEM_LEN);
	~SharedMemory();
	auto create()->HRESULT;
	auto open()->HRESULT;
	auto ptr()->uint8_t*;
	auto length()->uint32_t;
	auto close()->void;
private:
	std::wstring shared_memory_name;
	HANDLE mapping_handle = nullptr;
	uint8_t* shared_memory_ptr = nullptr;
	uint32_t shared_memory_len = 0;
};

_IPC_END