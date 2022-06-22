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
    SipAgent sa(socket);
    sa.Run(&count);
}