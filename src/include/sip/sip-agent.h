#pragma once
#include "sip-message.h"
#include "socket/raw-socket.h"
#include "rtp/rtp-socket.h"
#include "codecs/g711u.h"
#include <thread>
#include <functional>
class SipAgent
{
public:
    void Serv(std::shared_ptr<SIPMessage> msg){
        if (msg->type() == SIPMessage::MessageType::REQUEST){
            auto req = std::dynamic_pointer_cast<Request>(msg);
            if (auto func = handlers_.find(req->getMethod());func != handlers_.end()){
                func->second(msg);
            } else {
                ServDefault(msg);
            }
        }
    }
    void ServDefault(std::shared_ptr<SIPMessage> msg){
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
        while (true)
        {
            RawSocket::RecvData rd = socket_.Recv();
            auto sipMessage = SIPParser(rd.data).parse();
            (*count)++;
            Serv(sipMessage);
        }
    }
    void addHandler(const std::string& method, std::function<void(std::shared_ptr<SIPMessage>)> func){
        handlers_[method] = func;
    }
    void reply(int code, std::shared_ptr<SIPMessage> msg){
        auto req = std::dynamic_pointer_cast<Request>(msg);
        socket_.Send(req->genReply(code)->toString(), "192.168.8.201", 5061);
    }
    void print(std::shared_ptr<SIPMessage> msg){
        std::cout << *msg << std::endl;
    }
private:
    int rtpPortHi_{10000}, rtpPortLo_{15000}; // port range.
    int nextPort_{10000}; // next port for rtp
    RawSocket socket_; // the sip socket.

    std::map<std::string, std::function<void(std::shared_ptr<SIPMessage>)>> handlers_;
    
};