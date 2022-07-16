#pragma once
#include <memory>
#include <string>
#include <vector>
#include "message-header.h"
#include "sip-message.h"
#include "status-line.h"

class Response : public SIPMessage {
 public:
  Response(const StatusLine &sl);
  Response(int code, std::string status, std::vector<MessageHeader> headers);
  Response(int code, std::string status, const std::vector<MessageHeader> headers, const std::string &body);

  int StatusCode() const;
  void AddHeaders(const MessageHeader &mh);
  virtual bool _equal(const SIPMessage &other) const override;
  virtual std::string toString() const override;
  virtual MessageType type() const;
  virtual std::ostream &_print(std::ostream &os) const override;
  virtual std::shared_ptr<SIPMessage> genReply(int code) override;
  virtual std::shared_ptr<SIPMessage> genReply(int code, const std::string &body) override;
  std::string &body();

 private:
  int statusCode_;
  std::string statusDesc_;
  std::string body_;
};