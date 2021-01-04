#include "pch.h"
#include "win_semaphore.h"
_IPC_BEGIN

Semaphore::Semaphore(const std::wstring& name) {
}

Semaphore::~Semaphore() {
	this->close();
}

auto Semaphore::create() ->HRESULT {

}

auto Semaphore::open() -> HRESULT {
	return E_NOTIMPL;
}

auto Semaphore::wait() -> HRESULT {
	return E_NOTIMPL;
}

auto Semaphore::release() -> void {
	ReleaseSemaphore(this->semaphore, 1, nullptr);
}
auto Semaphore::close() ->void {
	if (this->semaphore) {
		CloseHandle(this->semaphore);
		this->semaphore = nullptr;
	}
}

_IPC_END
