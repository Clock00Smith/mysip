#pragma once
#include <string>
#include <memory>
struct SipDialog {
  std::string callID_;
  bool hasMedia_;
  bool done_; // if this safe to remove.
  std::shared_ptr<RTPSocket> rtpSocket_;
  std::shared_ptr<std::thread> rtpThread_;
};