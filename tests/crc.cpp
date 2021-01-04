#include "../ipc/src/crc/crc32.h"
auto crc_test()->void {
	std::string s = "abcdefg";
	std::cout << TinyIPC::crc32((const uint8_t*)s.data(), s.length(), CRC32_INIT) << std::endl;
	std::cout << TinyIPC::crc32((const uint8_t*)s.data(), s.length(), CRC32_INIT) << std::endl;
}