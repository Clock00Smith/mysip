#include "sip/response.h"
Response::Response(const StatusLine &sl) : statusCode_(sl.statusCode()), statusDesc_(sl.reasonPhrase()) {}
Response::Response(int code, std::string status, std::vector<MessageHeader> headers)
    : statusCode_(code), statusDesc_(std::move(status)), SIPMessage(std::move(headers)) {
  headers_.emplace_back(MessageHeader("Content-Length", "0"));
}

Response::Response(int code, std::string status, std::vector<MessageHeader> headers, const std::string &body)
    : statusCode_(code), statusDesc_(std::move(status)), SIPMessage(std::move(headers)), body_(body) {
  headers_.emplace_back(MessageHeader("Content-Length", std::to_string(body.size())));
  headers_.emplace_back(MessageHeader("Content-Type", "application/sdp"));
}
int Response::StatusCode() const { return statusCode_; }
void Response::AddHeaders(const MessageHeader &mh) { headers_.push_back(mh); }
std::string &Response::body() { return body_; }
bool Response::_equal(const SIPMessage &other) const {
  // TODO(clock)
  return false;
}

std::string Response::toString() const {
  {
    std::string val;
    val += "SIP/2.0 " + std::to_string(statusCode_) + " " + statusDesc_ + "\r\n";
    for (const auto &header : headers_) {
      val += header.name() + ": " + header.data() + "\r\n";
    }
    val += "\r\n";
    val += body_ + "\r\n";
    return val;
  }
}
Response::MessageType Response::type() const { return MessageType::RESPONSE; }

std::ostream &Response::_print(std::ostream &os) const { return os << toString(); }

std::shared_ptr<SIPMessage> Response::genReply(int code) {
  return std::make_shared<Response>(100, "Trying", std::vector<MessageHeader>());
}

// DONT
std::shared_ptr<SIPMessage> Response::genReply(int code, const std::string &body) { return genReply(100); }
