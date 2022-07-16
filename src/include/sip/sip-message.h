#pragma once
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <vector>
#include "sip/message-header.h"
class SIPMessage {
 public:
  SIPMessage();
  SIPMessage(std::vector<MessageHeader> headers);
  void AddHeaders(const MessageHeader &mh);
  std::optional<MessageHeader> getHeader(const std::string &name) const;
  enum MessageType { RAW, REQUEST, RESPONSE };
  bool operator==(const SIPMessage &other) const;
  virtual bool _equal(const SIPMessage &other) const = 0;
  virtual std::shared_ptr<SIPMessage> genReply(int code) = 0;
  virtual std::shared_ptr<SIPMessage> genReply(int code, const std::string &body) = 0;
  virtual std::ostream &_print(std::ostream &os) const = 0;
  virtual std::string toString() const = 0;
  virtual MessageType type() const;
  friend std::ostream &operator<<(std::ostream &os, const SIPMessage &obj);

 protected:
  std::vector<MessageHeader> headers_;
};
