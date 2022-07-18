#include "sip/message-header.h"

MessageHeader::MessageHeader(std::string name, std::string data) : name_(std::move(name)), data_(std::move(data)) {}

bool MessageHeader::operator==(const MessageHeader &other) const {
  return name_ == other.name_ && data_ == other.data_;
}

bool MessageHeader::operator!=(const MessageHeader &other) const { return !(*this == other); }
std::ostream &operator<<(std::ostream &os, const MessageHeader &obj) { return os << obj.name_ << ": " << obj.data_; }
std::string MessageHeader::name() const { return name_; }

std::string MessageHeader::data() const { return data_; }

ViaHeader::ViaHeader(std::string name, std::string data, std::string sentProtocol, std::string host, int port,
                     std::string viaParams)
    : MessageHeader(name, data),
      sentProtocol_(std::move(sentProtocol)),
      host_(std::move(host)),
      port_(port),
      viaParams_(std::move(viaParams)) {}

const std::string ViaHeader::host() const { return host_; }
const int ViaHeader::port() const { return port_; }