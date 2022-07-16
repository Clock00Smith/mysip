#pragma once
#include <memory>
#include <string>
struct SipDialog {
  std::string callID_;
  std::string uri_;
  bool hasMedia_;
  bool done_;  // if this safe to remove.
  std::shared_ptr<RTPSocket> rtpSocket_;
  std::shared_ptr<std::thread> rtpThread_;
};