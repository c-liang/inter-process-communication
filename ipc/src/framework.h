#pragma once

#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#include <Windows.h>
#include <assert.h>

#include <algorithm>
#include <cstdint>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

// stream
#include <iostream>

// multiple thread
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

#define _IPC_BEGIN namespace TinyIPC {
#define _IPC_END }
#define _IPC ::TinyIPC::

#define _IPC_MESSAGE_PACKING 4

// shared memory default size 8M

#define _IPC_SHARED_MEM_LEN 1024 * 1024 * 8

#define _IPC_SEMAPHORE_MAX 32
//#define _IPC_CHECK_CRC
// crc32 init num
static constexpr uint32_t CRC32_INIT = 0x6315749;

// 1.1
static constexpr uint32_t IPC_VERSION = 1 << 16 | 1;
