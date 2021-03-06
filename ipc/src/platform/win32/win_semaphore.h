#pragma once
#include "framework.h"

_IPC_BEGIN

class Semaphore {
public:
	Semaphore(std::wstring const& const name);
	~Semaphore();
	auto create()->HRESULT;
	auto open()->HRESULT;
	auto wait()->HRESULT;
	auto wait_timeout(const uint32_t timeout)->HRESULT;
	auto release()->void;
	auto close()->void;
private:
	std::wstring semaphore_name;
	HANDLE semaphore = nullptr;
};

_IPC_END