#include "socket/raw-socket.h"

RawSocket::RawSocket(std::string host, int port) : host_(std::move(host)), port_(port) {
  fd_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd_ > 0) {
    struct sockaddr_in local_addr;
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(port_);
    socklen_t len = sizeof(local_addr);
    int status = bind(fd_, reinterpret_cast<struct sockaddr *>(&local_addr), len);
    if (status < 0){
      throw SocketException("bind fail.");
    }
    timeval tv = {1, 0};
    setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
  } else {
    std::cout << "Oh no" << std::endl;
    throw SocketException("creating fail.");
  }
}
RawSocket::~RawSocket() {
  std::cout << "closing: " << port() << std::endl;
  close(fd_);
}
RawSocket::RecvData RawSocket::Recv(const size_t mtu) {
  char buffer[mtu];
  struct sockaddr_in peer_addr;
  socklen_t len = sizeof(peer_addr);
  ssize_t n = recvfrom(fd_, buffer, mtu, 0, reinterpret_cast<struct sockaddr *>(&peer_addr), &len);
  if (n > 0) {
    RecvData rd{inet_ntoa(peer_addr.sin_addr), peer_addr.sin_port, std::string(buffer, n)};
    return rd;
  }
  return RawSocket::RecvData{};
}

void RawSocket::Send(const std::string &data, const std::string &host, int port) {
  char buffer[data.size()];
  struct sockaddr_in peer_addr;
  peer_addr.sin_family = AF_INET;
  inet_aton(host.c_str(), &peer_addr.sin_addr);
  peer_addr.sin_port = htons(port);
  socklen_t len = sizeof(peer_addr);
  sendto(fd_, data.c_str(), data.size(), 0, reinterpret_cast<struct sockaddr *>(&peer_addr), len);
}

int RawSocket::port() const { return port_; }