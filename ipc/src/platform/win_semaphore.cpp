#include "pch.h"
#include "win_semaphore.h"
_IPC_BEGIN

auto Semaphore::release() -> void {
	ReleaseSemaphore(this->semphore, 1, nullptr);
}

_IPC_END
