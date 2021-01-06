#include "../ipc/src/message/message_pipe.h"

static const wchar_t* name = L"test123456";
static HANDLE QUIT_EVENT = nullptr;
static u32 const MAX_SEND = 100000;
static u32 RECEIVED = 0;
void send() {
  TinyIPC::MessagePipe pipe(name);
  pipe.open();

  for (auto i = 0; i < MAX_SEND; ++i) {
    std::string msg = "hello world " + std::to_string(i);
    // msg.resize(1024 * 1024 * 2);
    pipe.send_msg((const uint8_t*)msg.data(), msg.size() + 1);
  }
  pipe.close();
  Sleep(5000);
  SetEvent(QUIT_EVENT);
}
class MsgRecv : public TinyIPC::MessagePipe::RecvCallback {
  void received(uint8_t* data, uint32_t len) override {
    RECEIVED++;
    // std::cout << "len: " << len << " nums: " << num++ << std::endl;
    /*
    std::string s;
    s.append((char*)data, len);
    std::cout << s << " num: " << num++ << std::endl;*/
  }
};
void recv() {
  TinyIPC::MessagePipe pipe(name);
  MsgRecv recv;
  pipe.create(&recv);
  // pipe.create(nullptr);
  WaitForSingleObject(QUIT_EVENT, 1000000);
  pipe.close();
  assert(MAX_SEND == RECEIVED, "");
}

void message_pipe_test() {
  QUIT_EVENT = CreateEventW(nullptr, FALSE, FALSE, nullptr);
  std::thread r(recv);
  Sleep(100);
  std::thread s(send);
  r.join();
  s.join();
}