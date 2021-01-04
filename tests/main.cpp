#include <iostream>
#include <string>
#include "../ipc/src/crc32.h"
auto main()->int {
	std::string s = "abcdefg";
	std::cout << TinyIPC::crc32((const uint8_t*)s.data(), s.length(), CRC32_INIT) << std::endl;
	std::cout << TinyIPC::crc32((const uint8_t*)s.data(), s.length(), CRC32_INIT) << std::endl;
	return 0;
}