#pragma once
#include <string>

class StatusLine {
 public:
  StatusLine(std::string sipVersion, int statusCode, std::string reasonPhrase);
  int statusCode() const;
  std::string reasonPhrase() const;

 private:
  std::string sipVersion_;
  int statusCode_;
  std::string reasonPhrase_;
};