#pragma once
#include "../framework.h"
#include <cstdint>
_IPC_BEGIN
auto crc32(const uint8_t* data, size_t len, uint32_t init)->uint32_t;
_IPC_END