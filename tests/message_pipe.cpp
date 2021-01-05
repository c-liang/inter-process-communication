#include "../ipc/src/message/message_pipe.h"
static const wchar_t* name = L"test123456";
static HANDLE QUIT_EVENT = nullptr;
void send() {
  TinyIPC::MessagePipe pipe(name);
  pipe.open();
  std::vector<uint8_t> msg{'a', 'b', 'c', 'd', 'a', 'b', 'c',
                           'd', 'a', 'b', 'c', 'd', '\0'};
  msg.resize(1024);
  for (auto i = 0; i < 1000000; ++i) {
    pipe.send_msg(msg.data(), msg.size());
  }
  pipe.close();
  Sleep(5000);
  SetEvent(QUIT_EVENT);
}
void recv() {
  TinyIPC::MessagePipe pipe(name);
  pipe.create();
  WaitForSingleObject(QUIT_EVENT, 1000000);
  pipe.close();
}

void message_pipe_test() {
  QUIT_EVENT = CreateEventW(nullptr, FALSE, FALSE, nullptr);
  std::thread r(recv);
  Sleep(100);
  std::thread s(send);
  r.join();
  s.join();
}