#pragma once
#include "sip-parser.h"
#include "socket/raw-socket.h"
#include "rtp/rtp-socket.h"
#include "codecs/g711u.h"
#include <thread>
#include <functional>
#include <lua5.3/lua.hpp>
#include <map>
#include <iostream>
class SipAgent
{
public:
    void Serv(std::shared_ptr<SIPMessage> msg);
    void ServDefault(std::shared_ptr<SIPMessage> msg);

    SipAgent(RawSocket socket);
    SipAgent(RawSocket socket, int lo, int hi);
    void Run(int *count);
    void addHandler(const std::string &method, std::function<void(std::shared_ptr<SIPMessage>)> func);
    void reply(int code, std::shared_ptr<SIPMessage> msg);
    void replyWithMedia(int code, std::shared_ptr<SIPMessage> msg, std::string codec);
    void print(std::shared_ptr<SIPMessage> msg);
    void stop();
    void log(const std::string &msg);
    void doLua(const std::string &path, std::shared_ptr<SIPMessage> msg);

private:
    int rtpPortHi_{10000}, rtpPortLo_{15000}; // port range.
    int nextPort_{10000};                     // next port for rtp
    bool running_{false};
    RawSocket socket_; // the sip socket.
    std::map<std::string, std::function<void(std::shared_ptr<SIPMessage>)>> handlers_;
};

