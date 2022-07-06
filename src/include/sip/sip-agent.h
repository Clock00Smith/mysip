#pragma once
#include "sip-parser.h"
#include "socket/raw-socket.h"
#include "rtp/rtp-socket.h"
#include "codecs/g711u.h"
#include <thread>
#include <functional>
#include <lua5.3/lua.hpp>
#include <map>
#define SIP_AGENT_MT "mt.sipagent"
static int l_reply(lua_State *L);
static luaL_Reg SA_META[] = {
    {"reply", l_reply},
    {nullptr, nullptr},
};
class SipAgent
{
public:
    static constexpr int a = 1;
    void Serv(std::shared_ptr<SIPMessage> msg)
    {
        if (msg->type() == SIPMessage::MessageType::REQUEST)
        {
            auto req = std::dynamic_pointer_cast<Request>(msg);
            if (auto func = handlers_.find(req->getMethod()); func != handlers_.end())
            {
                func->second(msg);
            }
            else
            {
                ServDefault(msg);
            }
        }
    }
    void ServDefault(std::shared_ptr<SIPMessage> msg)
    {
        std::cout << "Got msg but have no handler for it. " << std::endl;
        std::cout << *msg << std::endl;
    }

    SipAgent(RawSocket socket) : socket_(socket) {}
    SipAgent(RawSocket socket, int lo, int hi) : socket_(socket), rtpPortLo_(lo), rtpPortHi_(hi)
    {
        nextPort_ = lo;
    }
    void Run(int *count)
    {
        running_ = true;
        while (running_)
        {
            RawSocket::RecvData rd = socket_.Recv();
            if (rd.data.size() == 0)
            {
                continue;
            }
            auto sipMessage = SIPParser(rd.data).parse();
            (*count)++;
            Serv(sipMessage);
        }
    }
    void addHandler(const std::string &method, std::function<void(std::shared_ptr<SIPMessage>)> func)
    {
        handlers_[method] = func;
    }
    void reply(int code, std::shared_ptr<SIPMessage> msg)
    {
        auto req = std::dynamic_pointer_cast<Request>(msg);
        socket_.Send(req->genReply(code)->toString(), "192.168.56.101", 5061);
    }
    void replyWithMedia(int code, std::shared_ptr<SIPMessage> msg, std::string codec){
        auto req = std::dynamic_pointer_cast<Request>(msg);
        SDP sdp = SDP::GetSDPWithCodec(codec, req->getHeader("Call-ID").data());
        auto rep = req->genReply(code, sdp.toString());
        socket_.Send(rep->toString(), "192.168.56.101", 5061);
    }
    void print(std::shared_ptr<SIPMessage> msg)
    {
        std::cout << *msg << std::endl;
    }
    void stop()
    {
        running_ = false;
    }
    void log(const std::string &msg)
    {
        std::cout << msg << std::endl;
    }
    void doLua(const std::string &path, std::shared_ptr<SIPMessage> msg)
    {
        lua_State *L = luaL_newstate();
        luaL_openlibs(L);
        luaL_newmetatable(L, SIP_AGENT_MT);
        lua_pushvalue(L, -1);
        lua_setfield(L, -2, "__index");
        luaL_setfuncs(L, SA_META, 0);

        SipAgent** tmp = (SipAgent**)lua_newuserdata(L, sizeof(SipAgent*));
        *tmp = this;
        luaL_setmetatable(L, SIP_AGENT_MT);
        lua_setglobal(L, "sa");
        std::shared_ptr<SIPMessage>** msgTmp = (std::shared_ptr<SIPMessage>**)lua_newuserdata(L, sizeof(std::shared_ptr<SIPMessage>*));
        *msgTmp = &msg;
        lua_setglobal(L, "msg");
        int res = luaL_dofile(L, path.c_str());
        if (res != LUA_OK){
            std::cout << "BAD: " << lua_tostring(L, -1) << std::endl;
        }
        lua_close(L);
    }

private:
    int rtpPortHi_{10000}, rtpPortLo_{15000}; // port range.
    int nextPort_{10000};                     // next port for rtp
    bool running_{false};
    RawSocket socket_; // the sip socket.
    std::map<std::string, std::function<void(std::shared_ptr<SIPMessage>)>> handlers_;
};

static int l_reply(lua_State *L)
{
    // int status = lua_tointeger(L, -1);
    if (lua_gettop(L) < 3){
        std::cout << "error: reply take two args." << std::endl;
        return 0;
    }
    std::shared_ptr<SIPMessage> **msg = (std::shared_ptr<SIPMessage> **)lua_touserdata(L, 3);
    int status = lua_tointeger(L, 2);
    SipAgent **sa = (SipAgent **)luaL_checkudata(L, 1, SIP_AGENT_MT);
    (*sa)->reply(status, *(*msg));
}
