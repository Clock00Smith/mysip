#pragma once
#include <string>

class StatusLine {
 public:
  StatusLine(const std::string &statusLine, int statusCode, const std::string &reasonPhrase);
  int statusCode() const;
  std::string reasonPhrase() const;

 private:
  std::string sipVersion_;
  int statusCode_;
  std::string reasonPhrase_;
};