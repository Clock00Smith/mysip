#pragma once
#include <memory>
#include <ostream>
#include <string>
class MessageHeader {
 public:
  MessageHeader(const std::string &name, const std::string &data);
  bool operator==(const MessageHeader &other) const;
  bool operator!=(const MessageHeader &other) const;
  friend std::ostream &operator<<(std::ostream &os, const MessageHeader &obj);
  std::string name() const;
  std::string data() const;

 private:
  std::string name_;
  std::string data_;
};