#include "sip/sip-parser.h"
#include "sip/sip-agent.h"
#include "socket/raw-socket.h"
#include "lua5.3/lua.hpp"
#include "utils/mylua.h"
#include <csignal>
int count = 0;
void signalHandler(int signum)
{
    std::cout << "q to quit." << std::endl;
}
int main()
{
    signal(SIGINT, signalHandler);
    std::cout << "bringing up lua." << std::endl;
    lua_State *L = luaL_newstate();
    luaL_requiref(L, "utils", luaopen_mylua, 1);
    luaL_dostring(L, "utils.c_log()");
    lua_close(L);
    std::cout << "bring up sip." << std::endl;
    RawSocket socket("0.0.0.0", 5060);
    SipAgent sa(socket, 10000, 20000);
    std::thread t([&sa]()
                  {
        sa.addHandler("INVITE", [&sa](std::shared_ptr<SIPMessage> msg)
                      {
        // sa.print(msg);
        sa.reply(100, msg);
        sa.reply(200, msg); });
        sa.addHandler("ACK", [&sa](std::shared_ptr<SIPMessage> msg) { // sa.print(msg); 
        });
            sa.addHandler("BYE", [&sa](std::shared_ptr<SIPMessage> msg)
                          {
        // sa.print(msg);
        sa.reply(200, msg); });
            sa.Run(&count); });
    char c;
    while (c != 'q')
    {
        std::cout << "q to quit." << std::endl;
        std::cin >> c;
    }
    sa.stop();
    t.join();
    std::cout << "packet recv: " << count << std::endl;
}