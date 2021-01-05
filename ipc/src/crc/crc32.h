#pragma once
#include "../framework.h"
#include <cstdint>
_IPC_BEGIN
auto crc32(const uint8_t* const data, const size_t len, const uint32_t init)->uint32_t;
_IPC_END