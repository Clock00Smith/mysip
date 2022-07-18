#pragma once
#include <memory>
#include <ostream>
#include <string>
class MessageHeader {
 public:
  MessageHeader() = default;
  MessageHeader(std::string name, std::string data);
  bool operator==(const MessageHeader &other) const;
  bool operator!=(const MessageHeader &other) const;
  friend std::ostream &operator<<(std::ostream &os, const MessageHeader &obj);
  std::string name() const;
  std::string data() const;
  virtual ~MessageHeader() = default;

 protected:
  std::string name_;
  std::string data_;
};

class ViaHeader : public MessageHeader {
 public:
  ViaHeader(std::string name, std::string data, std::string sentProtocol, std::string host, int port, std::string viaParams);
  const std::string host() const;
  const int port() const;
 private:
  std::string sentProtocol_;
  std::string host_;
  int port_;
  std::string viaParams_;
};