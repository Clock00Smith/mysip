#include "sip/request.h"

Request::Request(const RequestLine &rl) : rl_(rl) {}
void Request::AddHeaders(const MessageHeader &mh) { headers_.push_back(mh); }

bool Request::operator==(const Request &other) const {
  if (rl_ == other.rl_ && headers_.size() == other.headers_.size()) {
    for (size_t i = 0; i < headers_.size(); i++) {
      if (headers_[i] != other.headers_[i]) {
        return false;
      }
    }
    if (body_ == other.body_) {
      return true;
    } else {
      return false;
    }
  }
  return false;
}

std::string &Request::body() { return body_; }

std::string Request::getMethod() const { return rl_.method(); }

MessageHeader Request::getHeader(const std::string &name) const {
  for (auto itr = headers_.cbegin(); itr != headers_.cend(); itr++) {
    if (itr->name() == name) {
      return *itr;
    }
  }
}
std::shared_ptr<SIPMessage> Request::genReply(int code) { return genReply(code, ""); }

std::shared_ptr<SIPMessage> Request::genReply(int code, const std::string &body) {
  std::vector<MessageHeader> headers;
  headers.push_back(getHeader("Via"));
  headers.push_back(getHeader("From"));
  headers.push_back(getHeader("To"));
  headers.push_back(getHeader("Call-ID"));
  headers.push_back(getHeader("CSeq"));
  switch (code) {
    case 100: {
      return std::make_shared<Response>(code, "Trying", headers);
    }
    case 200: {
      if (getMethod() == "INVITE") {
        if (body != "") {
          return std::make_shared<Response>(code, "OK", headers, body);
        } else {
          return std::make_shared<Response>(code, "OK", headers);
        }
      } else {
        return std::make_shared<Response>(code, "OK", headers);
      }
    }
  }
  if (STATUS_DESC.find(code) == STATUS_DESC.cend()) {
    return std::make_shared<Response>(code, "UNKNOWN", headers_);
  }
  return std::make_shared<Response>(code, STATUS_DESC[code], headers_);
}

bool Request::_equal(const SIPMessage &other) const { return this->operator==(dynamic_cast<const Request &>(other)); }

std::ostream &Request::_print(std::ostream &os) const

{
  return os << *this;
}

std::string Request::toString() const { return "TODO"; }

Request::MessageType Request::type() const { return MessageType::REQUEST; }

std::ostream &operator<<(std::ostream &os, const Request &obj) {
  os << std::endl << obj.rl_ << std::endl;
  for (auto itr = obj.headers_.cbegin(); itr != obj.headers_.cend(); itr++) {
    os << *itr << std::endl;
  }
  os << std::endl << obj.body_ << std::endl;
  return os;
}