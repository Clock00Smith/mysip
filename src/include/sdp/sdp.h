#pragma once
#include <atomic>
#include <mutex>
#include <string>
class SDP {
 public:
  static SDP GetSDPWithCodec(const std::string &codec, const std::string &call_id);
  std::string toString() const;

 private:
  static int nextPort;
  static std::mutex portLock;
  std::string raw_;
  int port_;
  SDP(int port);
};
