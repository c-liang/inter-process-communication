#pragma once
#include "framework.h"

_IPC_BEGIN

class Semaphore {
public:
	Semaphore(std::string& name);
	auto wait()->HRESULT;
	auto release()->void;
private:
	std::string semaphore_name;
	HANDLE semphore;
};

_IPC_END