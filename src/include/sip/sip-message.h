#pragma once
#include <memory>
#include <ostream>
#include <string>
class SIPMessage {
 public:
  enum MessageType { RAW, REQUEST, RESPONSE };
  bool operator==(const SIPMessage &other) const;
  virtual bool _equal(const SIPMessage &other) const = 0;
  virtual std::shared_ptr<SIPMessage> genReply(int code) = 0;
  virtual std::shared_ptr<SIPMessage> genReply(int code, const std::string &body) = 0;
  virtual std::ostream &_print(std::ostream &os) const = 0;
  virtual std::string toString() const = 0;
  virtual MessageType type() const;
  friend std::ostream &operator<<(std::ostream &os, const SIPMessage &obj);
};
