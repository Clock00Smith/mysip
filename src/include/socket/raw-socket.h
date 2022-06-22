#pragma once

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>

class RawSocket
{
public:
    struct RecvData
    {
        std::string fromHost;
        int fromPort;
        std::string data;
    };

    RawSocket(const std::string &host, int port) : host_(host), port_(port)
    {
        fd_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (fd_ > 0)
        {
            struct sockaddr_in localAddr;
            localAddr.sin_family = AF_INET;
            localAddr.sin_addr.s_addr = INADDR_ANY;
            localAddr.sin_port = htons(port_);
            socklen_t len = sizeof(localAddr);
            bind(fd_, reinterpret_cast<struct sockaddr *>(&localAddr), len);
        }
    }
    RecvData Recv(const size_t mtu = 65535)
    {
        char *buffer = new char[mtu];
        struct sockaddr_in peerAddr;
        socklen_t len = sizeof(peerAddr);
        int n = recvfrom(fd_, buffer, mtu, 0, reinterpret_cast<struct sockaddr *>(&peerAddr), &len);
        RecvData rd{inet_ntoa(peerAddr.sin_addr), peerAddr.sin_port, std::string(buffer, n)};
        #ifdef DEBUG
            std::cout << rd.fromHost << ":" << rd.fromPort << std::endl;
            std::cout << rd.data << std::endl;
        #endif 
        return rd;
    }
    void Send(const std::string &data, const std::string &host, int port)
    {
        char *buffer = new char[data.size()];
        struct sockaddr_in peerAddr;
        peerAddr.sin_family = AF_INET;
        inet_aton(host.c_str(), &peerAddr.sin_addr);
        peerAddr.sin_port = htons(port);
        socklen_t len = sizeof(peerAddr);
        #ifdef DEBUG
            std::cout << data << std::endl;
        #endif
        sendto(fd_, data.c_str(), data.size(), 0, reinterpret_cast<struct sockaddr *>(&peerAddr), len);
    }

private:
    int fd_;
    std::string host_;
    int port_;
};
