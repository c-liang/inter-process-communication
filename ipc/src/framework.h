#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <Windows.h>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cstdint>

//stream
#include <iostream>

//multiple thread
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#define _IPC_BEGIN namespace TinyIPC {
#define _IPC_END   }
#define _IPC       ::TinyIPC::

#define _IPC_MESSAGE_PACKING 4

//shared memory default size 16M

#define _IPC_SHARED_MEM_LEN 1024*1024*16

//crc32 init num
static constexpr uint32_t CRC32_INIT = 0x6315749;

