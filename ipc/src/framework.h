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

#ifdef _WIN32
#endif

#include "config.h"

// define types as rust
using i8 = int8_t;
using u8 = uint8_t;
using i16 = int16_t;
using u16 = uint16_t;
using i32 = int32_t;
using u32 = uint32_t;
using i64 = int64_t;
using u64 = uint64_t;
using f64 = double;
using f32 = float;
