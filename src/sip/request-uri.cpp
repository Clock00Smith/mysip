#include "sip/request-uri.h"
#include <iostream>
RequestURI::RequestURI(std::string data) {
  size_t last = 0;
  size_t ptr = 0;
  size_t user_info_start = 0;
  size_t host_start = 0;
  size_t host_end = 0;
  while (ptr < data.size()) {
    if (data[ptr] == ':') {
      if (user_info_start == 0) {
        user_info_start = ptr;

      } else {
        host_end = ptr;
      }
    }
    if (data[ptr] == '@') {
      host_start = ptr;
    }
    ptr++;
  }
  userInfo_ = data.substr(user_info_start + 1, host_start - user_info_start - 1);
  if (host_end == 0) {
    host_ = data.substr(host_start + 1);
    port_ = 5060;
  } else {
    host_ = data.substr(host_start + 1, host_end - host_start - 1);
    port_ = std::atoi(data.substr(host_end + 1).c_str());
  }
}

RequestURI::RequestURI(std::string userInfo, std::string host, int port)
    : userInfo_(std::move(userInfo)), host_(std::move(host)), port_(port) {}

std::string RequestURI::toString() const { return "sip:" + userInfo_ + "@" + host_ + ":" + std::to_string(port_); }
bool RequestURI::operator==(const RequestURI &other) const { return toString() == other.toString(); }

std::ostream &operator<<(std::ostream &os, const RequestURI &obj) { return os << obj.toString(); }

const std::string RequestURI::host() const { return host_; }

const int RequestURI::port() const { return port_; }
