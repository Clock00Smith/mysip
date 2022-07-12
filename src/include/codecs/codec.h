#pragma once
#include <string>
class Codec {
 public:
  virtual ~Codec(){};
  virtual std::string decode(const std::string &data) = 0;
  virtual std::string encode(const std::string &data) = 0;
};