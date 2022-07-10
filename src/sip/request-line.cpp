#include "sip/request-line.h"

RequestLine::RequestLine(const std::string &method, RequestURI uri, const std::string &sipVersion)
    : method_(method), uri_(uri), sipVersion_(sipVersion) {}

std::string RequestLine::method() const { return method_; }

bool RequestLine::operator==(const RequestLine &other) const {
  return method_ == other.method_ && sipVersion_ == other.sipVersion_ && uri_ == other.uri_;
}

std::ostream &operator<<(std::ostream &os, const RequestLine &obj) {
  return os << obj.method_ << " " << obj.uri_ << " " << obj.sipVersion_ << std::endl;
}