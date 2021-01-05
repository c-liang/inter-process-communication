#pragma once
#include "../../framework.h"

_IPC_BEGIN
class MutexLockGuard;
class Mutex {
 public:
  Mutex(std::wstring const& const name);
  ~Mutex();
  auto create() -> HRESULT;
  auto open() -> HRESULT;
  auto lock() -> HRESULT;
  auto try_lock(uint32_t timeout) -> HRESULT;
  auto unlock() -> void;
  auto close() -> void;

 private:
  std::wstring mutex_name;
  HANDLE mutex = nullptr;
};

class MutexLockGuard {
 public:
  MutexLockGuard(Mutex& mutex, uint32_t timeout = INFINITE) : mutex(mutex) {
    this->lock_result = mutex.try_lock(timeout);
  };
  auto locked() -> bool const { return SUCCEEDED(this->lock_result); }
  ~MutexLockGuard() {
    if (this->locked()) {
      mutex.unlock();
    }
  };

 private:
  Mutex& mutex;
  HRESULT lock_result;
};

_IPC_END