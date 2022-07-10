#include "socket/raw-socket.h"

RawSocket::RawSocket(const std::string &host, int port) : host_(host), port_(port) {
  fd_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd_ > 0) {
    struct sockaddr_in localAddr;
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = INADDR_ANY;
    localAddr.sin_port = htons(port_);
    socklen_t len = sizeof(localAddr);
    bind(fd_, reinterpret_cast<struct sockaddr *>(&localAddr), len);
    timeval tv = {1, 0};
    setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
  } else {
    std::cout << "Oh no" << std::endl;
    throw SocketException("creating fail.");
  }
}

RawSocket::RecvData RawSocket::Recv(const size_t mtu) {
  char buffer[mtu];
  struct sockaddr_in peerAddr;
  socklen_t len = sizeof(peerAddr);
  int n = recvfrom(fd_, buffer, mtu, 0, reinterpret_cast<struct sockaddr *>(&peerAddr), &len);
  if (n > 0) {
    RecvData rd{inet_ntoa(peerAddr.sin_addr), peerAddr.sin_port, std::string(buffer, n)};
    return rd;
  }
  return RawSocket::RecvData{};
}

void RawSocket::Send(const std::string &data, const std::string &host, int port) {
  char buffer[data.size()];
  struct sockaddr_in peerAddr;
  peerAddr.sin_family = AF_INET;
  inet_aton(host.c_str(), &peerAddr.sin_addr);
  peerAddr.sin_port = htons(port);
  socklen_t len = sizeof(peerAddr);
  sendto(fd_, data.c_str(), data.size(), 0, reinterpret_cast<struct sockaddr *>(&peerAddr), len);
}

int RawSocket::port() const { return port_; }