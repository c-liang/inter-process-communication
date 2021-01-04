#include "pch.h"
#include "win_mutex.h"

_IPC_BEGIN

Mutex::Mutex(const std::wstring& name) :mutex_name(name) {
}
Mutex::~Mutex() {
	this->close();
}

auto Mutex::create() ->HRESULT {
	mutex = CreateMutexW(nullptr, FALSE, this->mutex_name.c_str());
	if (mutex == nullptr) {
		return HRESULT_FROM_WIN32(GetLastError());
	}
	else {
		ReleaseMutex(mutex);
	}
	return S_OK;
}

auto Mutex::open() ->HRESULT {
	mutex = OpenMutexW(MUTEX_ALL_ACCESS, FALSE, this->mutex_name.c_str());
	if (mutex == nullptr) {
		return HRESULT_FROM_WIN32(GetLastError());
	}
	else {
		return S_OK;
	}
}

auto Mutex::lock() ->HRESULT {
	WaitForSingleObject(this->mutex, INFINITE);
	return S_OK;
}

auto Mutex::unlock() ->void
{
	ReleaseMutex(mutex);
}

auto Mutex::close() -> void {
	if (this->mutex) {
		CloseHandle(this->mutex);
		this->mutex = nullptr;
	}
}

_IPC_END
