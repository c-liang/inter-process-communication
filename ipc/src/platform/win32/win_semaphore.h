#pragma once
#include "framework.h"

_IPC_BEGIN

class Semaphore {
public:
	Semaphore(const std::wstring& name);
	~Semaphore();
	auto create()->HRESULT;
	auto open()->HRESULT;
	auto wait()->HRESULT;
	auto release()->void;
	auto close()->void;
private:
	std::wstring semaphore_name;
	HANDLE semaphore = nullptr;
};

_IPC_END