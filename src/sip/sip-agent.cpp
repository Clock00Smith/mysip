#include "sip/sip-agent.h"

#define SIP_AGENT_MT "mt.sipagent"
static int l_reply(lua_State *L);
static int l_reply_with_media(lua_State *L);
static luaL_Reg SA_META[] = {
    {"reply", l_reply},
    {"replyWithMedia", l_reply_with_media},
    {nullptr, nullptr},
};

void SipAgent::Serv(std::shared_ptr<SIPMessage> msg) {
  if (msg->type() == SIPMessage::MessageType::REQUEST) {
    auto req = std::dynamic_pointer_cast<Request>(msg);
    if (auto func = handlers_.find(req->getMethod()); func != handlers_.end()) {
      func->second(msg);
    } else {
      ServDefault(msg);
    }
  }
}

void SipAgent::ServDefault(std::shared_ptr<SIPMessage> msg) {
  std::cout << "Got msg but have no handler for it. " << std::endl;
  std::cout << *msg << std::endl;
}

SipAgent::SipAgent(const std::string &host, int port) : socket_(host, port) {}

SipAgent::SipAgent(const std::string &host, int port, int lo, int hi) : socket_(host, port), rtpPortLo_(lo), rtpPortHi_(hi) {
  nextPort_ = lo;
}

void SipAgent::Run(int *count) {
  running_ = true;
  while (running_) {
    RawSocket::RecvData rd = socket_.Recv();
    if (rd.data.size() == 0) {
      continue;
    }
    auto sipMessage = SIPParser(rd.data).parse();
    (*count)++;
    Serv(sipMessage);
  }
}

void SipAgent::addHandler(const std::string &method, std::function<void(std::shared_ptr<SIPMessage>)> func) {
  handlers_[method] = func;
}

void SipAgent::reply(int code, std::shared_ptr<SIPMessage> msg) {
  auto req = std::dynamic_pointer_cast<Request>(msg);
  socket_.Send(req->genReply(code)->toString(), "192.168.56.101", 5061);
}

void SipAgent::replyWithMedia(int code, std::shared_ptr<SIPMessage> msg, std::string codec) {
  auto req = std::dynamic_pointer_cast<Request>(msg);
  std::string callId = req->getHeader("Call-ID").data();
  SDP sdp = SDP::GetSDPWithCodec(codec, req->getHeader("Call-ID").data());
  SipDialog sipDialog;
  sipDialog.callID_ = req->getHeader("Call-ID").data();
  sipDialog.hasMedia_ = true;
  sipDialog.rtpSocket_ = sdp.getSocket();
  std::thread rtpThread([](std::shared_ptr<RTPSocket> sock) { sock->Run(); }, sipDialog.rtpSocket_);
  rtpThread.detach();
  dialogs_[callId] = sipDialog;
  std::cout << callId << " : using port: " << sipDialog.rtpSocket_->port() << std::endl;

  auto rep = req->genReply(code, sdp.toString());
  socket_.Send(rep->toString(), "192.168.56.101", 5061);
}

void SipAgent::print(std::shared_ptr<SIPMessage> msg) { std::cout << *msg << std::endl; }

void SipAgent::stop() { running_ = false; }

void SipAgent::log(const std::string &msg) { std::cout << msg << std::endl; }

void SipAgent::endDialog(std::shared_ptr<SIPMessage> &msg) {
  auto req = std::dynamic_pointer_cast<Request>(msg);
  std::string callId = req->getHeader("Call-ID").data();
  if (auto itr = dialogs_.find(callId); itr != dialogs_.end()) {
    std::cout << "removing " << callId << " on port: " << dialogs_[callId].rtpSocket_->port() << std::endl;
    dialogs_[callId].rtpSocket_->Stop();
    dialogs_.erase(callId);
  } else {
    std::cout << "no " << callId << " found." << std::endl;
  }
}
void SipAgent::doLua(const std::string &path, std::shared_ptr<SIPMessage> msg) {
  lua_State *L = luaL_newstate();
  luaL_openlibs(L);
  luaL_newmetatable(L, SIP_AGENT_MT);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  luaL_setfuncs(L, SA_META, 0);

  SipAgent **tmp = (SipAgent **)lua_newuserdata(L, sizeof(SipAgent *));
  *tmp = this;
  luaL_setmetatable(L, SIP_AGENT_MT);
  lua_setglobal(L, "sa");
  std::shared_ptr<SIPMessage> **msgTmp =
      (std::shared_ptr<SIPMessage> **)lua_newuserdata(L, sizeof(std::shared_ptr<SIPMessage> *));
  *msgTmp = &msg;
  lua_setglobal(L, "msg");
  int res = luaL_dofile(L, path.c_str());
  if (res != LUA_OK) {
    std::cout << "BAD: " << lua_tostring(L, -1) << std::endl;
  }
  lua_close(L);
}

static int l_reply(lua_State *L) {
  // int status = lua_tointeger(L, -1);
  if (lua_gettop(L) < 3) {
    std::cout << "error: reply take two args." << std::endl;
    return 0;
  }
  std::shared_ptr<SIPMessage> **msg = (std::shared_ptr<SIPMessage> **)lua_touserdata(L, 3);
  int status = lua_tointeger(L, 2);
  SipAgent **sa = (SipAgent **)luaL_checkudata(L, 1, SIP_AGENT_MT);
  (*sa)->reply(status, *(*msg));
}

static int l_reply_with_media(lua_State *L) {
  if (lua_gettop(L) < 4) {
    std::cout << "error: reply with media take 3 args." << std::endl;
    return 0;
  }
  std::string codecName = lua_tostring(L, 4);
  std::shared_ptr<SIPMessage> **msg = (std::shared_ptr<SIPMessage> **)lua_touserdata(L, 3);
  int statusCode = lua_tointeger(L, 2);
  SipAgent **sa = (SipAgent **)luaL_checkudata(L, 1, SIP_AGENT_MT);
  (*sa)->replyWithMedia(statusCode, *(*msg), codecName);
}