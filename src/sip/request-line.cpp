#include "sip/request-line.h"

RequestLine::RequestLine(std::string method, RequestURI uri, std::string sipVersion)
    : method_(std::move(method)), uri_(std::move(uri)), sipVersion_(std::move(sipVersion)) {}

std::string RequestLine::method() const { return method_; }

bool RequestLine::operator==(const RequestLine &other) const {
  return method_ == other.method_ && sipVersion_ == other.sipVersion_ && uri_ == other.uri_;
}

std::ostream &operator<<(std::ostream &os, const RequestLine &obj) {
  return os << obj.method_ << " " << obj.uri_ << " " << obj.sipVersion_ << std::endl;
}

std::string RequestLine::toString() const { return method_ + " " + uri_.toString() + " " + sipVersion_; }