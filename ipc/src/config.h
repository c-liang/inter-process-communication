#pragma once
#include <cstdint>

#define _IPC_BEGIN namespace TinyIPC {
#define _IPC_END }
#define _IPC ::TinyIPC::

// message struct pack size
#define _IPC_MESSAGE_PACKING 4
// shared memory size 8M
#define _IPC_SHARED_MEM_LEN 1024 * 1024 * 8
// semaphore count
#define _IPC_SEMAPHORE_MAX 32
// check message data crc when received a message
#define _IPC_CHECK_CRC
// create a thread to handle received message
#define _IPC_MSG_HEANLD_THREAD
// pipe max pending message when send message failed
#define _IPC_MAX_PENDING_MESSAGE 256
// crc32 init num
static constexpr uint32_t CRC32_INIT = 0x6315749;
// ipc version major: 1 min: 1
static constexpr uint32_t IPC_VERSION = 1 << 16 | 1;