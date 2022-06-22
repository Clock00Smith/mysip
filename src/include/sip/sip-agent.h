#pragma once
#include "sip-message.h"
#include "socket/raw-socket.h"
#include <thread>
class SipAgent
{
public:
    void Serv(std::shared_ptr<SIPMessage> msg)
    {
        if (msg->type() == SIPMessage::MessageType::REQUEST)
        {
            Request *req = dynamic_cast<Request *>(msg.get());
            if (req->getMethod() == "INVITE")
            {
                std::string data = req->genReply(100)->toString();
                socket_.Send(data, "192.168.8.201", 5061);
                socket_.Send(req->genReply(200)->toString(), "192.168.8.201", 5061);
            }
            else if (req->getMethod() == "ACK")
            {
            }
            else if (req->getMethod() == "BYE")
            {
                std::string data = req->genReply(200)->toString();
                socket_.Send(data, "192.168.8.201", 5061);
            }
            else
            {
                std::cout << "Unrecognized method: " << req->getMethod() << std::endl;
            }
        }
    }
    SipAgent(RawSocket socket) : socket_(socket)
    {
    }
    void Run(int *count)
    {
        while (true)
        {
            RawSocket::RecvData rd = socket_.Recv();
            auto sipMessage = SIPParser(rd.data).parse();
            Serv(sipMessage);
        }
    }

private:
    RawSocket socket_;
};