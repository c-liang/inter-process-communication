#include "../ipc/src/message/message_pipe.h"

static const wchar_t* name = L"test123456";
static HANDLE QUIT_EVENT = nullptr;
void send() {
  TinyIPC::MessagePipe pipe(name);
  pipe.open();

  for (auto i = 0; i < 10000; ++i) {
    std::string msg =
        "hello world ello world ello world ello world " + std::to_string(i);
    pipe.send_msg((const uint8_t*)msg.data(), msg.size() + 1);
  }
  pipe.close();
  Sleep(5000);
  SetEvent(QUIT_EVENT);
}

class MsgRecv : public TinyIPC::MessagePipe::RecvCallback {
  void received(uint8_t* data, uint32_t len) override {
    std::string s;
    s.append((char*)data, len);
    std::cout << s << std::endl;
  }
};
void recv() {
  TinyIPC::MessagePipe pipe(name);
  MsgRecv recv;
  pipe.create(&recv);
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