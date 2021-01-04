#pragma once
#include "framework.h"

_IPC_BEGIN

class Semaphore {
public:
	Semaphore(const std::wstring& name);
	auto create()->HRESULT;
	auto open()->HRESULT;
	auto wait()->HRESULT;
	auto release()->void;
private:
	std::wstring semaphore_name;
	HANDLE semphore;
};

_IPC_END