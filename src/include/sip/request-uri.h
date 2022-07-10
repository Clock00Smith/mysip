#pragma once
#include <string>

class RequestURI {
 public:
  RequestURI(std::string data);
  bool operator==(const RequestURI &other) const;
  friend std::ostream &operator<<(std::ostream &os, const RequestURI &obj);

 private:
  std::string raw_;
};