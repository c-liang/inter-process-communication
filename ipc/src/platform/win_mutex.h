#pragma once
#include "../framework.h"

_IPC_BEGIN
class MutexLockGuard;
class Mutex {
public:
	Mutex(const std::wstring& name);
	~Mutex();
	auto create()->HRESULT;
	auto open()->HRESULT;
	auto lock()->HRESULT;
	auto unlock()->void;
private:
	std::wstring mutex_name;
	HANDLE mutex = nullptr;
};

class MutexLockGuard {
public:
	MutexLockGuard(Mutex& mutex) :mutex(mutex) { mutex.lock(); };
	~MutexLockGuard() { mutex.unlock(); };
private:
	Mutex& mutex;
};
_IPC_END