#include "sip/request.h"

Request::Request(RequestLine request_line) : rl_(std::move(request_line)) {}

bool Request::operator==(const Request &other) const {
  if (rl_ == other.rl_ && headers_.size() == other.headers_.size()) {
    for (size_t i = 0; i < headers_.size(); i++) {
      if (headers_[i] != other.headers_[i]) {
        return false;
      }
    }
    return body_ == other.body_;
  }
  return false;
}

std::string Request::body() const { return body_; }
void Request::setBody(const std::string &body) {
  AddHeaders(std::make_shared<MessageHeader>("Content-Type", "application/sdp"));
  AddHeaders(std::make_shared<MessageHeader>("Content-Length", std::to_string(body.size())));
  body_ = body;
}
std::string Request::getMethod() const { return rl_.method(); }

std::shared_ptr<SIPMessage> Request::genReply(int code) { return genReply(code, ""); }

std::shared_ptr<SIPMessage> Request::genReply(int code, const std::string &body) {
  std::vector<std::shared_ptr<MessageHeader>> headers;
  headers.push_back(GetHeader("Via").value());
  headers.push_back(GetHeader("From").value());
  headers.push_back(GetHeader("To").value());
  headers.push_back(GetHeader("Call-ID").value());
  headers.push_back(GetHeader("CSeq").value());
  switch (code) {
    case 100: {
      return std::make_shared<Response>(code, "Trying", headers);
    }
    case 200: {
      if (getMethod() == "INVITE") {
        if (!body.empty()) {
          return std::make_shared<Response>(code, "OK", headers, body);
        }
        return std::make_shared<Response>(code, "OK", headers);
      }
      return std::make_shared<Response>(code, "OK", headers);
    }
  }
  if (STATUS_DESC.find(code) == STATUS_DESC.cend()) {
    return std::make_shared<Response>(code, "UNKNOWN", headers_);
  }
  return std::make_shared<Response>(code, STATUS_DESC[code], headers_);
}

bool Request::_equal(const SIPMessage &other) const { return this->operator==(dynamic_cast<const Request &>(other)); }

std::ostream &Request::_print(std::ostream &os) const { return os << *this; }

std::string Request::toString() const {
  std::string val;
  val += rl_.toString() + "\r\n";
  for (auto &mh : headers_) {
    val += mh->name() + ": " + mh->data() + "\r\n";
  }
  val += "\r\n";
  val += body_ + "\r\n";
  return val;
}

Request::MessageType Request::type() const { return MessageType::REQUEST; }

std::ostream &operator<<(std::ostream &os, const Request &obj) {
  os << std::endl << obj.rl_ << std::endl;
  for (const auto &header : obj.headers_) {
    os << header << std::endl;
  }
  os << std::endl << obj.body_ << std::endl;
  return os;
}

const RequestLine &Request::requestLine() const { return rl_; }