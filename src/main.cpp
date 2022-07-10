#include <csignal>
#include "lua5.3/lua.hpp"
#include "sip/sip-agent.h"
#include "socket/raw-socket.h"
int count = 0;
void signalHandler(int signum) {
  std::cout << "q to quit." << std::endl;
  exit(0);
}
int main() {
  signal(SIGINT, signalHandler);
  std::cout << "bringing up lua." << std::endl;
  std::cout << "bring up sip." << std::endl;
  RawSocket socket("0.0.0.0", 5060);
  SipAgent sa(socket, 10000, 20000);
  std::thread t([&sa]() {
    sa.addHandler("INVITE", [&sa](std::shared_ptr<SIPMessage> msg) {
      sa.reply(100, msg);
      sa.reply(183, msg);
      sa.replyWithMedia(200, msg, "G711U");
    });
    sa.addHandler("CANCEL", [&sa](std::shared_ptr<SIPMessage> msg) { sa.reply(200, msg); });
    sa.addHandler("ACK", [&sa](std::shared_ptr<SIPMessage> msg) {});
    sa.addHandler("BYE", [&sa](std::shared_ptr<SIPMessage> msg) { sa.reply(200, msg); });
    sa.Run(&count);
  });
  char c;
  while (c != 'q') {
    std::cout << "q to quit." << std::endl;
    std::cin >> c;
  }
  sa.stop();
  t.join();
  std::cout << "packet recv: " << count << std::endl;
}