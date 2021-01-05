#include "win_mutex.h"

#include "pch.h"

_IPC_BEGIN

Mutex::Mutex(std::wstring const& const name) : mutex_name(name) {}
Mutex::~Mutex() {
  this->close();
}

auto Mutex::create() -> HRESULT {
  SECURITY_ATTRIBUTES sa;
  SECURITY_DESCRIPTOR sd;
  InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
  SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);
  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.bInheritHandle = TRUE;
  sa.lpSecurityDescriptor = &sd;

  mutex = CreateMutexW(&sa, FALSE, this->mutex_name.c_str());
  if (mutex == nullptr) {
    return HRESULT_FROM_WIN32(GetLastError());
  } else {
    ReleaseMutex(mutex);
  }
  return S_OK;
}

auto Mutex::open() -> HRESULT {
  mutex = OpenMutexW(MUTEX_ALL_ACCESS, FALSE, this->mutex_name.c_str());
  if (mutex == nullptr) {
    return HRESULT_FROM_WIN32(GetLastError());
  } else {
    return S_OK;
  }
}

auto Mutex::lock() -> HRESULT {
  WaitForSingleObject(this->mutex, INFINITE);
  return S_OK;
}

auto Mutex::try_lock(uint32_t timeout) -> HRESULT {
  if (WaitForSingleObject(this->mutex, timeout) == WAIT_OBJECT_0) {
    return S_OK;
  } else {
    return HRESULT_FROM_WIN32(ERROR_TIMEOUT);
  }
}

auto Mutex::unlock() -> void {
  ReleaseMutex(mutex);
}

auto Mutex::close() -> void {
  if (this->mutex) {
    CloseHandle(this->mutex);
    this->mutex = nullptr;
  }
}

_IPC_END
