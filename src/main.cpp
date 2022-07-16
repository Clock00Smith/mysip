#include <csignal>
#include "lua5.3/lua.hpp"
#include "sip/sip-agent.h"
#include "socket/raw-socket.h"
int count = 0;
void SignalHandler(int signum) {
  std::cout << "q to quit." << std::endl;
  exit(0);
}
int main() {
  signal(SIGINT, SignalHandler);
  std::cout << "bringing up lua." << std::endl;
  std::cout << "bring up sip." << std::endl;
  SipAgent sip_agent("0.0.0.0", 5060, 10000, 20000);
  std::thread sip_agent_thr([&sip_agent]() {
    sip_agent.addRequestHandler("INVITE", [&sip_agent](const std::shared_ptr<SIPMessage>& msg) { sip_agent.doLua("../scripts/invite.lua", msg); });
    sip_agent.addRequestHandler("CANCEL", [&sip_agent](const std::shared_ptr<SIPMessage>& msg) { sip_agent.doLua("../scripts/cancel.lua", msg); });
    sip_agent.addRequestHandler("ACK", [&sip_agent](const std::shared_ptr<SIPMessage>& msg) {});
    sip_agent.addRequestHandler("BYE", [&sip_agent](const std::shared_ptr<SIPMessage>& msg) {
      sip_agent.doLua("../scripts/bye.lua", msg);
      sip_agent.endDialog(msg);
    });
    sip_agent.Run(&count);
  });
  char input_char;
  while (input_char != 'q') {
    switch (input_char) {
      case 'i':
        sip_agent.invite("sip:9196@192.168.56.101:5061", "sip:1000@192.168.56.101:5060");
        break;
    }
    std::cout << "q to quit." << std::endl;
    std::cin >> input_char;
  }
  sip_agent.stop();
  sip_agent_thr.join();
  std::cout << "packet recv: " << count << std::endl;
}