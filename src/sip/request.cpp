#include "sip/request.h"

Request::Request(const RequestLine &rl) : rl_(rl) {}

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

std::string Request::body() const { return body_; }
void Request::setBody(const std::string &body) {
  AddHeaders(MessageHeader("Content-Type", "application/sdp"));
  AddHeaders(MessageHeader("Content-Length", std::to_string(body.size())));
  body_ = body;
}
std::string Request::getMethod() const { return rl_.method(); }

std::shared_ptr<SIPMessage> Request::genReply(int code) { return genReply(code, ""); }

std::shared_ptr<SIPMessage> Request::genReply(int code, const std::string &body) {
  std::vector<MessageHeader> headers;
  headers.push_back(getHeader("Via").value());
  headers.push_back(getHeader("From").value());
  headers.push_back(getHeader("To").value());
  headers.push_back(getHeader("Call-ID").value());
  headers.push_back(getHeader("CSeq").value());
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

std::string Request::toString() const {
  std::string val = "";
  val += rl_.toString() + "\r\n";
  for (auto mh : headers_) {
    val += mh.name() + ": " + mh.data() + "\r\n";
  }
  val += "\r\n";
  val += body_ + "\r\n";
  return val;
}

Request::MessageType Request::type() const { return MessageType::REQUEST; }

std::ostream &operator<<(std::ostream &os, const Request &obj) {
  os << std::endl << obj.rl_ << std::endl;
  for (auto itr = obj.headers_.cbegin(); itr != obj.headers_.cend(); itr++) {
    os << *itr << std::endl;
  }
  os << std::endl << obj.body_ << std::endl;
  return os;
}