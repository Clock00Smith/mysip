#pragma once
#include <memory>
#include <string>
#include <vector>
#include "message-header.h"
#include "sip-message.h"

class Response : public SIPMessage {
 public:
  Response(int code, const std::string &status, std::vector<MessageHeader> headers);
  Response(int code, const std::string &status, const std::vector<MessageHeader> headers, const std::string &body);

  int code() const;
  virtual bool _equal(const SIPMessage &other) const override;
  virtual std::string toString() const override;
  virtual MessageType type() const;
  virtual std::ostream &_print(std::ostream &os) const override;
  virtual std::shared_ptr<SIPMessage> genReply(int code) override;
  virtual std::shared_ptr<SIPMessage> genReply(int code, const std::string &body) override;

 private:
  int statusCode_;
  std::string statusDesc_;
  std::vector<MessageHeader> headers_;
  std::string body_;
};