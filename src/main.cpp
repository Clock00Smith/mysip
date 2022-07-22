#include <csignal>
#include "lua5.3/lua.hpp"
#include "sip/sip-agent.h"
#include "socket/raw-socket.h"
#include "utils/command.h"
int count = 0;
void SignalHandler(int signum) {
  std::cout << "q to quit." << std::endl;
  exit(0);
}
int main(int argc, char* args[]) {
  if (argc != 2){
    std::cout << "Need the local binding sip uri." << std::endl;
    return 0;
  }
  signal(SIGINT, SignalHandler);
  std::cout << "bringing up lua." << std::endl;
  std::cout << "bring up sip." << std::endl;
  RequestURI uri(args[1]);
  SipAgent sip_agent(uri.host(), uri.port(), 10000, 20000);
  sip_agent.setFromURI(args[1]);
  std::thread sip_agent_thr([&sip_agent]() {
    sip_agent.addRequestHandler("INVITE", [&sip_agent](const std::shared_ptr<SIPMessage> &msg) {
      sip_agent.doLua("../scripts/invite.lua", msg);
    });
    sip_agent.addRequestHandler("CANCEL", [&sip_agent](const std::shared_ptr<SIPMessage> &msg) {
      sip_agent.doLua("../scripts/cancel.lua", msg);
    });
    sip_agent.addRequestHandler("ACK", [&sip_agent](const std::shared_ptr<SIPMessage> &msg) {});
    sip_agent.addRequestHandler("BYE", [&sip_agent](const std::shared_ptr<SIPMessage> &msg) {
      sip_agent.doLua("../scripts/bye.lua", msg);
      sip_agent.endDialog(msg);
    });
    sip_agent.Run(&count);
  });
  char input_char[100] = {0};
  while (true) {
    Command command(input_char);
    if (command.command() == "invite") {
      if (command.params().empty()){
        std::cout << "Need a to uri like sip:1000@127.0.0.1:5060" << std::endl;
      } else {
        sip_agent.invite(command.params());
      }
    } else if (command.command() == "quit") {
      break;
    } else {
      std::cout << "q to quit." << std::endl;
    }

    std::cin.getline(input_char, 100);
  }

  sip_agent.stop();
  sip_agent_thr.join();
  std::cout << "packet recv: " << count << std::endl;
}