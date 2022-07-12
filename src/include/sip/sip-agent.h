#pragma once
#include <functional>
#include <iostream>
#include <lua5.3/lua.hpp>
#include <map>
#include <thread>
#include "codecs/g711u.h"
#include "sdp/sdp.h"
#include "sip-parser.h"
#include "socket/raw-socket.h"
#include "sip/sip-dialog.h"
class SipAgent {
 public:
  void Serv(std::shared_ptr<SIPMessage> msg);
  void ServDefault(std::shared_ptr<SIPMessage> msg);

  SipAgent(const std::string& host, int port);
  SipAgent(const std::string& host, int port, int lo, int hi);
  void Run(int *count);
  void addHandler(const std::string &method, std::function<void(std::shared_ptr<SIPMessage>)> func);
  void reply(int code, std::shared_ptr<SIPMessage> msg);
  void replyWithMedia(int code, std::shared_ptr<SIPMessage> msg, std::string codec);
  void print(std::shared_ptr<SIPMessage> msg);
  void stop();
  void log(const std::string &msg);
  void doLua(const std::string &path, std::shared_ptr<SIPMessage> msg);
  void endDialog(std::shared_ptr<SIPMessage>& msg);
 private:
  int rtpPortHi_{10000}, rtpPortLo_{15000};  // port range.
  int nextPort_{10000};                      // next port for rtp
  bool running_{false};
  RawSocket socket_;  // the sip socket.
  std::map<std::string, std::function<void(std::shared_ptr<SIPMessage>)>> handlers_;
  // use call-id to identify the dialog for now, this is wrong, but we keep it simple.
  // actually we make it way too simple: only record those with media here, and stop the media thread once recv a BYE.
  std::unordered_map<std::string, SipDialog> dialogs_; 
 
};
