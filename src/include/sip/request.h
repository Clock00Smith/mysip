#pragma once
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <vector>
#include "request-line.h"
#include "response.h"
#include "sip-message.h"

class Request : public SIPMessage {
 public:
  std::map<int, std::string> STATUS_DESC = {{200, "OK"}, {100, "Trying"}};
  Request(RequestLine request_line);
  bool operator==(const Request &other) const;
  std::string body() const;
  void setBody(const std::string &body);
  std::string getMethod() const;
  virtual std::shared_ptr<SIPMessage> genReply(int code) override;
  virtual std::shared_ptr<SIPMessage> genReply(int code, const std::string &body) override;
  virtual bool _equal(const SIPMessage &other) const override;
  virtual std::ostream &_print(std::ostream &os) const override;
  virtual std::string toString() const override;
  virtual MessageType type() const override;
  friend std::ostream &operator<<(std::ostream &os, const Request &obj);
  const RequestLine& requestLine() const;
 private:
  RequestLine rl_;
  std::string body_;
};