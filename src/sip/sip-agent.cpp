#include "sip/sip-agent.h"

#define SIP_AGENT_MT "mt.sipagent"
static int LSAReply(lua_State *L);
static int LSAReplyWithMedia(lua_State *L);
static luaL_Reg sa_meta[] = {
    {"reply", LSAReply},
    {"replyWithMedia", LSAReplyWithMedia},
    {nullptr, nullptr},
};

void SipAgent::Serv(const std::shared_ptr<SIPMessage> &msg) {
  if (msg->type() == SIPMessage::MessageType::REQUEST) {
    auto req = std::dynamic_pointer_cast<Request>(msg);
    if (auto func = requestHandlers_.find(req->getMethod()); func != requestHandlers_.end()) {
      func->second(msg);
    } else {
      ServDefault(msg);
    }
  } else {
    auto res = std::dynamic_pointer_cast<Response>(msg);
    if (res->StatusCode() >= 400) {
      sendAck(res);
    } else if (res->StatusCode() >= 200) {
      sendAck(res);
    } else if (res->StatusCode() >= 100) {
      // we should change the state of the particular call to stop a retrans.
      // we dont have neither retrans nor state, just ignore.
      std::cout << "got a 1xx." << std::endl;
    } else {
      ServDefault(msg);
    }
  }
}
void SipAgent::sendAck(const std::shared_ptr<Response> &msg) {
  std::string call_id = msg->GetHeader("Call-ID").value()->data();
  std::cout << "This is a response for: " << call_id << std::endl;
  SipDialog sip_dialog = dialogs_[call_id];
  std::thread rtp_thread([](const std::shared_ptr<RTPSocket> &sock) { sock->Run(); }, sip_dialog.rtpSocket_);
  rtp_thread.detach();
  Request req(RequestLine("ACK", *sip_dialog.uri_, "SIP/2.0"));
  req.AddHeaders(msg->GetHeader("Via").value());
  req.AddHeaders(msg->GetHeader("To").value());
  req.AddHeaders(msg->GetHeader("From").value());
  req.AddHeaders(MessageHeader("Max-Forwards", "70"));
  req.AddHeaders(MessageHeader("CSeq", "1 ACK"));
  req.AddHeaders(MessageHeader("Call-ID", call_id));
  req.AddHeaders(MessageHeader("Content-Length", "0"));
  socket_.Send(req.toString(), sip_dialog.uri_->host(), sip_dialog.uri_->port());
}
void SipAgent::ServDefault(const std::shared_ptr<SIPMessage> &msg) {
  std::cout << "Got msg but have no handler for it. " << std::endl;
  std::cout << *msg << std::endl;
}
SipAgent::SipAgent(const std::string &host, int port) : socket_(host, port) {
  std::time_t result = std::time(nullptr);
  nextCallID_.store(result);
}

SipAgent::SipAgent(const std::string &host, int port, int lo, int hi)
    : socket_(host, port), rtpPortLo_(lo), rtpPortHi_(hi) {
  nextPort_ = lo;
  std::time_t result = std::time(nullptr);
  nextCallID_.store(result);
}

void SipAgent::Run(int *count) {
  running_ = true;
  while (running_) {
    RawSocket::RecvData rd = socket_.Recv();
    if (rd.data.empty()) {
      continue;
    }
    auto sip_message = SIPParser(rd.data).parse();
    (*count)++;
    Serv(sip_message);
  }
}

void SipAgent::addRequestHandler(const std::string &method,
                                 const std::function<void(const std::shared_ptr<SIPMessage> &)> &func) {
  requestHandlers_[method] = func;
}

void SipAgent::reply(int code, const std::shared_ptr<SIPMessage> &msg) {
  auto req = std::dynamic_pointer_cast<Request>(msg);
  auto via = std::dynamic_pointer_cast<ViaHeader>(req->GetHeader("Via").value());
  std::cout << "Sending reply: " << code << " to " << via->host() << ":" << via->port() << std::endl;
  socket_.Send(req->genReply(code)->toString(), via->host(), via->port());
}

void SipAgent::replyWithMedia(int code, const std::shared_ptr<SIPMessage> &msg, const std::string &codec) {
  auto req = std::dynamic_pointer_cast<Request>(msg);
  auto via = std::dynamic_pointer_cast<ViaHeader>(req->GetHeader("Via").value());
  std::string call_id = req->GetHeader("Call-ID").value()->data();
  SDP sdp = SDP::GetSDPWithCodec(codec, req->GetHeader("Call-ID").value()->data());
  SipDialog sip_dialog;
  sip_dialog.callID_ = req->GetHeader("Call-ID").value()->data();
  sip_dialog.hasMedia_ = true;
  sip_dialog.rtpSocket_ = sdp.getSocket();
  std::thread rtp_thread([](const std::shared_ptr<RTPSocket> &sock) { sock->Run(); }, sip_dialog.rtpSocket_);
  rtp_thread.detach();
  dialogs_[call_id] = sip_dialog;
  std::cout << call_id << " : using port: " << sip_dialog.rtpSocket_->port() << std::endl;

  auto rep = req->genReply(code, sdp.toString());
  std::cout << "Sending reply: " << code << " to " << via->host() << ":" << via->port() << std::endl;
  socket_.Send(rep->toString(), via->host(), via->port());
}

void SipAgent::print(const std::shared_ptr<SIPMessage> &msg) { std::cout << *msg << std::endl; }

void SipAgent::stop() { running_ = false; }

void SipAgent::log(const std::string &msg) { std::cout << msg << std::endl; }
void SipAgent::setFromURI(const std::string &fromURI) {
  fromURI_ = fromURI;
}
void SipAgent::endDialog(const std::shared_ptr<SIPMessage> &msg) {
  auto req = std::dynamic_pointer_cast<Request>(msg);
  std::string call_id = req->GetHeader("Call-ID").value()->data();
  if (auto itr = dialogs_.find(call_id); itr != dialogs_.end()) {
    std::cout << "removing " << call_id << " on port: " << dialogs_[call_id].rtpSocket_->port() << std::endl;
    dialogs_[call_id].rtpSocket_->Stop();
    dialogs_.erase(call_id);
  } else {
    std::cout << "no " << call_id << " found." << std::endl;
  }
}
void SipAgent::invite(const std::string &toUri){
  invite(toUri, fromURI_);
}
void SipAgent::invite(const std::string &toUri, const std::string &fromUri) {
  std::string call_id = std::to_string(getNextCallID());
  Request req(RequestLine("INVITE", RequestURI(toUri), "SIP/2.0"));
  req.AddHeaders(MessageHeader("To", "<" + toUri + ">"));
  req.AddHeaders(MessageHeader("From", "<" + fromUri + ">"));
  req.AddHeaders(MessageHeader("Call-ID", call_id));
  req.AddHeaders(MessageHeader("Max-Forwards", "70"));
  req.AddHeaders(MessageHeader("Via", "SIP/2.0/UDP 192.168.56.101:5060;branch=z9hG4bK"));
  req.AddHeaders(MessageHeader("CSeq", "1 INVITE"));
  req.AddHeaders(MessageHeader("Contact", "<sip:my@192.168.56.101:5060>"));
  SDP sdp = SDP::GetSDPWithCodec("G711U", call_id);
  req.setBody(sdp.toString());
  SipDialog sip_dialog;
  sip_dialog.uri_ = std::make_shared<RequestURI>(toUri);
  sip_dialog.callID_ = call_id;
  sip_dialog.hasMedia_ = true;
  sip_dialog.rtpSocket_ = sdp.getSocket();
  // std::thread rtpThread([](std::shared_ptr<RTPSocket> sock) { sock->Run(); }, sipDialog.rtpSocket_);
  // rtpThread.detach();
  dialogs_[call_id] = sip_dialog;

  socket_.Send(req.toString(), req.requestLine().uri().host(), req.requestLine().uri().port());
  std::cout << req.toString() << std::endl;
}
void SipAgent::doLua(const std::string &path, std::shared_ptr<SIPMessage> msg) {
  lua_State *l = luaL_newstate();
  luaL_openlibs(l);
  luaL_newmetatable(l, SIP_AGENT_MT);
  lua_pushvalue(l, -1);
  lua_setfield(l, -2, "__index");
  luaL_setfuncs(l, sa_meta, 0);

  SipAgent **tmp = static_cast<SipAgent **>(lua_newuserdata(l, sizeof(SipAgent *)));
  *tmp = this;
  luaL_setmetatable(l, SIP_AGENT_MT);
  lua_setglobal(l, "sa");
  std::shared_ptr<SIPMessage> **msg_tmp =
      static_cast<std::shared_ptr<SIPMessage> **>(lua_newuserdata(l, sizeof(std::shared_ptr<SIPMessage> *)));
  *msg_tmp = &msg;
  lua_setglobal(l, "msg");
  int res = luaL_dofile(l, path.c_str());
  if (res != LUA_OK) {
    std::cout << "BAD: " << lua_tostring(l, -1) << std::endl;
  }
  lua_close(l);
}

static int LSAReply(lua_State *L) {
  // int status = lua_tointeger(L, -1);
  if (lua_gettop(L) < 3) {
    std::cout << "error: reply take two args." << std::endl;
    return 0;
  }
  std::shared_ptr<SIPMessage> **msg = static_cast<std::shared_ptr<SIPMessage> **>(lua_touserdata(L, 3));
  int status = lua_tointeger(L, 2);
  SipAgent **sa = static_cast<SipAgent **>(luaL_checkudata(L, 1, SIP_AGENT_MT));
  (*sa)->reply(status, *(*msg));
  return 0;
}

static int LSAReplyWithMedia(lua_State *L) {
  if (lua_gettop(L) < 4) {
    std::cout << "error: reply with media take 3 args." << std::endl;
    return 0;
  }
  std::string codec_name = lua_tostring(L, 4);
  std::shared_ptr<SIPMessage> **msg = static_cast<std::shared_ptr<SIPMessage> **>(lua_touserdata(L, 3));
  int status_code = lua_tointeger(L, 2);
  SipAgent **sa = static_cast<SipAgent **>(luaL_checkudata(L, 1, SIP_AGENT_MT));
  (*sa)->replyWithMedia(status_code, *(*msg), codec_name);
  return 0;
}

int SipAgent::getNextCallID() { return nextCallID_.fetch_add(1); }