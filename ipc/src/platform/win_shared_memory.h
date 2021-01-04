#pragma once
#include "../framework.h"
_IPC_BEGIN

class SharedMemory {
public:
	SharedMemory(const std::wstring name);
	auto open()->HRESULT;
	auto create()->HRESULT;
	auto get_ptr()->uint8_t*;
private:
	std::wstring shared_memory_name;
};

_IPC_END