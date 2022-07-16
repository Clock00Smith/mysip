#pragma once

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <exception>
#include <iostream>
#include <string>

class RawSocket {
 public:
  class SocketException : std::runtime_error {
   public:
    SocketException(const std::string &error) : std::runtime_error(error){};
  };
  struct RecvData {
    std::string fromHost;
    int fromPort;
    std::string data;
  };

  RawSocket(std::string host, int port);
  virtual ~RawSocket();
  RecvData Recv(const size_t mtu = 65535);
  void Send(const std::string &data, const std::string &host, int port);

  int port() const;

 private:
  int fd_;
  std::string host_;
  int port_;
};
