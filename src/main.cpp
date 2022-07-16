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
  SipAgent sa("0.0.0.0", 5060, 10000, 20000);
  std::thread t([&sa]() {
    sa.addRequestHandler("INVITE", [&sa](std::shared_ptr<SIPMessage> msg) { sa.doLua("../scripts/invite.lua", msg); });
    sa.addRequestHandler("CANCEL", [&sa](std::shared_ptr<SIPMessage> msg) { sa.doLua("../scripts/cancel.lua", msg); });
    sa.addRequestHandler("ACK", [&sa](std::shared_ptr<SIPMessage> msg) {});
    sa.addRequestHandler("BYE", [&sa](std::shared_ptr<SIPMessage> msg) {
      sa.doLua("../scripts/bye.lua", msg);
      sa.endDialog(msg);
    });
    sa.Run(&count);
  });
  char c;
  while (c != 'q') {
    switch (c) {
      case 'i':
        sa.invite("sip:9196@192.168.56.101:5061", "sip:1000@192.168.56.101:5060");
        break;
    }
    std::cout << "q to quit." << std::endl;
    std::cin >> c;
  }
  sa.stop();
  t.join();
  std::cout << "packet recv: " << count << std::endl;
}