#pragma once
#include <string>
// we only support sip:something@host:port
class RequestURI {
 public:
  RequestURI(std::string data);
  RequestURI(std::string userInfo, std::string host, int port);
  std::string toString() const;
  bool operator==(const RequestURI &other) const;
  friend std::ostream &operator<<(std::ostream &os, const RequestURI &obj);
  const std::string host() const;
  const int port() const;
 private:

  std::string userInfo_;
  std::string host_;
  int port_;
};