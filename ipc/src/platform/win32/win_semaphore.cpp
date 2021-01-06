#include "win_semaphore.h"
#include "pch.h"
_IPC_BEGIN

Semaphore::Semaphore(std::wstring const& const name) : semaphore_name(name) {}

Semaphore::~Semaphore() {
	this->close();
}

auto Semaphore::create() -> HRESULT {
	SECURITY_ATTRIBUTES sa;
	SECURITY_DESCRIPTOR sd;
	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = &sd;

	this->semaphore = CreateSemaphoreW(&sa, 0, _IPC_SEMAPHORE_MAX, this->semaphore_name.c_str());
	if (this->semaphore == nullptr) {
		return HRESULT_FROM_WIN32(GetLastError());
	}
	return S_OK;
}

auto Semaphore::open() -> HRESULT {
	this->semaphore =
		OpenSemaphoreW(SEMAPHORE_ALL_ACCESS, FALSE, this->semaphore_name.c_str());
	if (this->semaphore == nullptr) {
		return HRESULT_FROM_WIN32(GetLastError());
	}
	return S_OK;
}

auto Semaphore::wait() -> HRESULT {
	WaitForSingleObject(this->semaphore, INFINITE);
	return S_OK;
}

auto Semaphore::wait_timeout(const uint32_t timeout) -> HRESULT {
	if (WaitForSingleObject(this->semaphore, timeout) == WAIT_OBJECT_0) {
		return S_OK;
	}
	else {
		return HRESULT_FROM_WIN32(ERROR_TIMEOUT);
	}
}

auto Semaphore::release() -> void {
	ReleaseSemaphore(this->semaphore, 1, nullptr);
}
auto Semaphore::close() -> void {
	if (this->semaphore) {
		CloseHandle(this->semaphore);
		this->semaphore = nullptr;
	}
}

_IPC_END
