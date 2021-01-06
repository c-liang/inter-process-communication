#include <iostream>
#include <string>
#include "../ipc/src/crc/crc32.h"
extern auto serialization_test() -> void;
extern auto crc_test() -> void;
extern auto message_pipe_test() -> void;
auto main() -> int {
  serialization_test();
  message_pipe_test();
  crc_test();
  return 0;
}