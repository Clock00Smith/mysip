#include "sip/sip-message.h"
SIPMessage::SIPMessage() = default; 
SIPMessage::SIPMessage(std::vector<MessageHeader> headers) : headers_(std::move(headers)) {}
bool SIPMessage::operator==(const SIPMessage &other) const {
  if (typeid(*this) != typeid(other)) {
    return false;
  }
  return this->_equal(other);
}
void SIPMessage::AddHeaders(const MessageHeader &mh) { headers_.push_back(mh); }

std::optional<MessageHeader> SIPMessage::GetHeader(const std::string &name) const {
  for (const auto & header : headers_) {
    if (header.name() == name) {
      return header;
    }
  }
  return std::nullopt;
}
SIPMessage::MessageType SIPMessage::type() const { return MessageType::RAW; }

std::ostream &operator<<(std::ostream &os, const SIPMessage &obj) { return obj._print(os); }