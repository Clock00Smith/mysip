#include "sip/message-header.h"

MessageHeader::MessageHeader(std::string name, std::string data) : name_(std::move(name)), data_(std::move(data)) {}

bool MessageHeader::operator==(const MessageHeader &other) const {
  return name_ == other.name_ && data_ == other.data_;
}

bool MessageHeader::operator!=(const MessageHeader &other) const { return !(*this == other); }
std::ostream &operator<<(std::ostream &os, const MessageHeader &obj) { return os << obj.name_ << ": " << obj.data_; }
std::string MessageHeader::name() const { return name_; }

std::string MessageHeader::data() const { return data_; }