#include "sip/sip-message.h"
SIPMessage::SIPMessage() = default;
SIPMessage::SIPMessage(std::vector<std::shared_ptr<MessageHeader>> headers) : headers_(std::move(headers)) {}
bool SIPMessage::operator==(const SIPMessage &other) const {
  if (typeid(*this) != typeid(other)) {
    return false;
  }
  return this->_equal(other);
}
void SIPMessage::AddHeaders(std::shared_ptr<MessageHeader> mh) { headers_.push_back(mh); }

void SIPMessage::AddHeaders(MessageHeader mh){
  headers_.push_back(std::make_shared<MessageHeader>(mh));
}

std::optional<std::shared_ptr<MessageHeader>> SIPMessage::GetHeader(const std::string &name) const {
  for (const auto &header : headers_) {
    if (header->name() == name) {
      return header;
    }
  }
  return std::nullopt;
}
SIPMessage::MessageType SIPMessage::type() const { return MessageType::RAW; }

std::ostream &operator<<(std::ostream &os, const SIPMessage &obj) { return obj._print(os); }