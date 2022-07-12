#pragma once
#include <atomic>
#include <mutex>
#include <string>
#include <memory>

#include <codecs/g711u.h>
#include <rtp/rtp-socket.h>

class SDP {
 public:
  static SDP GetSDPWithCodec(const std::string &codec, const std::string &call_id);
  std::string toString() const;
  std::shared_ptr<RTPSocket> getSocket();
 private:
  static int nextPort;
  static std::mutex portLock;
  std::string raw_;
  int port_;
  std::shared_ptr<RTPSocket> audioSocket_;
  SDP(int port, std::shared_ptr<RTPSocket> audioSocket_);
};
