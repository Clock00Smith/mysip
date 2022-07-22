#pragma once
#include <functional>
#include <iostream>
#include <lua5.3/lua.hpp>
#include <map>
#include <thread>
#include <unordered_map>
#include "codecs/g711u.h"
#include "sdp/sdp.h"
#include "sip-parser.h"
#include "sip/sip-dialog.h"
#include "socket/raw-socket.h"
class SipAgent {
 public:
  void Serv(const std::shared_ptr<SIPMessage> &msg);
  void ServDefault(const std::shared_ptr<SIPMessage> &msg);

  SipAgent(const std::string &host, int port);
  SipAgent(const std::string &host, int port, int lo, int hi);
  void Run(int *count);
  void addRequestHandler(const std::string &method, const std::function<void(const std::shared_ptr<SIPMessage> &)> &func);
  void reply(int code, const std::shared_ptr<SIPMessage> &msg);
  void replyWithMedia(int code, const std::shared_ptr<SIPMessage> &msg, const std::string &codec);
  void print(const std::shared_ptr<SIPMessage> &msg);
  void stop();
  void log(const std::string &msg);
  void setFromURI(const std::string &fromURI);
  void doLua(const std::string &path, std::shared_ptr<SIPMessage> msg);
  void endDialog(const std::shared_ptr<SIPMessage> &msg);
  void invite(const std::string &toUri);
  void invite(const std::string &toUri, const std::string &fromUri);
  void sendAck(const std::shared_ptr<Response> &msg);
  
 private:
  std::atomic<int> nextCallID_;
  std::string fromURI_;
  int getNextCallID();
  int rtpPortHi_{15000}, rtpPortLo_{10000};  // port range.
  int nextPort_{10000};                      // next port for rtp
  bool running_{false};
  RawSocket socket_;  // the sip socket.
  std::map<std::string, std::function<void(std::shared_ptr<SIPMessage>)>> requestHandlers_;
  // use call-id to identify the dialog for now, this is wrong, but we keep it simple.
  // actually we make it way too simple: only record those with media here, and stop the media thread once recv a BYE.
  std::unordered_map<std::string, SipDialog> dialogs_;
};
