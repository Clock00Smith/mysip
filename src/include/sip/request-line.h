#pragma once
#include <ostream>
#include <string>
#include "request-uri.h"
class RequestLine {
 public:
  RequestLine(const std::string &method, RequestURI uri, const std::string &sipVersion);
  std::string method() const;
  std::string toString() const;
  bool operator==(const RequestLine &other) const;

  friend std::ostream &operator<<(std::ostream &os, const RequestLine &obj);

 private:
  std::string method_;
  RequestURI uri_;
  std::string sipVersion_;
};