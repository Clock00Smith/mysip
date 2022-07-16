#include "sip/sip-message.h"
SIPMessage::SIPMessage() {}
SIPMessage::SIPMessage(std::vector<MessageHeader> headers) : headers_(headers) {}
bool SIPMessage::operator==(const SIPMessage &other) const {
  if (typeid(*this) != typeid(other)) {
    return false;
  }
  return this->_equal(other);
}
void SIPMessage::AddHeaders(const MessageHeader &mh) { headers_.push_back(mh); }

std::optional<MessageHeader> SIPMessage::getHeader(const std::string &name) const {
  for (auto itr = headers_.cbegin(); itr != headers_.cend(); itr++) {
    if (itr->name() == name) {
      return *itr;
    }
  }
  return std::nullopt;
}
SIPMessage::MessageType SIPMessage::type() const { return MessageType::RAW; }

std::ostream &operator<<(std::ostream &os, const SIPMessage &obj) { return obj._print(os); }