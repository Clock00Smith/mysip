#include "sip/sip-parser.h"
#include "sip/sip-agent.h"
#include "socket/raw-socket.h"

#include <csignal>
int count = 0;
void signalHandler(int signum)
{
    std::cout << "Quit: " << count << std::endl;
    exit(signum);
}
int main()
{
    signal(SIGINT, signalHandler);
    RawSocket socket("0.0.0.0", 5060);
    SipAgent sa(socket, 10000, 20000);
    sa.addHandler("INVITE", [&sa](std::shared_ptr<SIPMessage> msg) {
        sa.print(msg);
        sa.reply(100, msg);
        sa.reply(200, msg); 
    });
    sa.addHandler("ACK", [&sa](std::shared_ptr<SIPMessage> msg){
        sa.print(msg);
    });
    sa.addHandler("BYE", [&sa](std::shared_ptr<SIPMessage> msg){
        sa.print(msg);
        sa.reply(200, msg);
    });
    sa.Run(&count);
}