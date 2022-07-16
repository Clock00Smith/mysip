#include "sip/sip-parser.h"

SIPParser::SIPParser(const std::string &data) : data_(data), cur_ptr_(0) {}

std::string SIPParser::read() {
  size_t tmp = cur_ptr_;
  cur_ptr_++;
  return std::string(data_.substr(tmp, 1));
}

char SIPParser::peek() {
  if (cur_ptr_ >= data_.size()) {
    // a throw here will be better.
    return '\0';
  }
  return data_[cur_ptr_];
}

char SIPParser::peek(size_t n) {
  if (cur_ptr_ + n - 1 >= data_.size()) {
    return '\0';
  }
  return data_[cur_ptr_ + n - 1];
}

bool SIPParser::match(char c, char expect) { return c == expect; }

void SIPParser::mustMatch(char c, char expect) {
  if (cur_ptr_ >= data_.size()) {
    throw UnmatchException(cur_ptr_, "");
  } else {
    if (data_[cur_ptr_] != expect) {
      throw UnmatchException(cur_ptr_, std::to_string(expect));
    }
  }
}

std::string SIPParser::readTillCRLF() {
  std::string val = "";
  while (peek() != '\r' && peek(2) != '\n') {
    val += read();
  }
  return val;
}

void SIPParser::eatSpaces() {
  while (peek() == SPACE) {
    read();
  }
}

std::shared_ptr<SIPMessage> SIPParser::parse() { return SIP_MESSAGE(); }

bool SIPParser::IsRequest() const {
  // TODO
  return false;
}

std::shared_ptr<SIPMessage> SIPParser::SIP_MESSAGE() {
  if (peek() == 'S' && peek(2) == 'I' && peek(3) == 'P') {
    return RESPONSE();
  } else {
    return REQUEST();
  }
};

bool SIPParser::IsMethod() {
  // TODO
  // Dont remember why I need this.
  return true;
}

void SIPParser::CRLF() {
  mustMatch(peek(), CR);
  read();
  mustMatch(peek(), LF);
  read();
}

void SIPParser::SP() {
  mustMatch(peek(), SPACE);
  read();
}

// Repuest - Line *(message - header) CRLF[message - body] std::shared_ptr<Request> REQUEST()
std::shared_ptr<Request> SIPParser::REQUEST() {
  if (IsMethod()) {
    RequestLine rl = REQUEST_LINE();
    Request req(rl);
    while (peek() != '\r') {
      MessageHeader mh = MESSAGE_HEADER();
      req.AddHeaders(mh);
      CRLF();
    }

    CRLF();
    req.body() = MESSAGE_BODY();
    return std::make_shared<Request>(req);
  }
}
std::shared_ptr<Response> SIPParser::RESPONSE() {
  // TODO
  std::vector<MessageHeader> headers;
  StatusLine sl = STATUS_LINE();
  Response res(sl);
  while (peek() != '\r') {
    MessageHeader mh = MESSAGE_HEADER();
    res.AddHeaders(mh);
    CRLF();
  }
  CRLF();
  res.body() = MESSAGE_BODY();
  return std::make_shared<Response>(res);
}

RequestLine SIPParser::REQUEST_LINE() {
  std::string method = METHOD();
  SP();
  RequestURI ruri = REQUEST_URI();
  SP();
  std::string sip_version = SIP_VERSION();
  CRLF();
  return RequestLine(method, ruri, sip_version);
}

std::string SIPParser::REQUEST_URI() {
  std::string val = "";
  // TODO
  // there is actually a rule for URI.
  // ignore it for now.
  while (peek() != SPACE) {
    if (peek() == '\0') {
      break;
    }
    val += read();
  }
  return val;
}

StatusLine SIPParser::STATUS_LINE() {
  std::string sip_version = SIP_VERSION();
  SP();
  int status_code = STATUS_CODE();
  SP();
  std::string reason_phrase = REASON_PHRASE();
  CRLF();
  return StatusLine(sip_version, status_code, reason_phrase);
}

int SIPParser::STATUS_CODE() {
  std::string val = "";
  for (int i = 0; i < 3; i++) {
    if (peek() < '0' && peek() > '9') {
      throw UnmatchException(cur_ptr_, "digit.");
    } else {
      val += read();
    }
  }
  return std::atoi(val.c_str());
}

std::string SIPParser::REASON_PHRASE() {
  std::string val = "";
  val += readTillCRLF();
  return val;
}

std::string SIPParser::INVITE() {
  for (char c : std::string("INVITE")) {
    mustMatch(peek(), c);
    read();
  }
  return "INVITE";
}
std::string SIPParser::ACK() {
  for (char c : std::string("ACK")) {
    mustMatch(peek(), c);
    read();
  }
  return "ACK";
}
std::string SIPParser::BYE() {
  for (char c : std::string("BYE")) {
    mustMatch(peek(), c);
    read();
  }
  return "BYE";
}
std::string SIPParser::CANCEL() {
  for (char c : std::string("CANCEL")) {
    mustMatch(peek(), c);
    read();
  }
  return "CANCEL";
}
std::string SIPParser::METHOD() {
  switch (peek()) {
    case 'I':
      return INVITE();
    case 'A':
      return ACK();
    case 'B':
      return BYE();
    case 'C':
      return CANCEL();
    default:
      return "";
  }
}

// SIP-Version = "SIP" "/" 1*DIGIT "." 1*DIGIT
std::string SIPParser::SIP_VERSION() {
  // "SIP" "/"
  for (char c : std::string("SIP/")) {
    mustMatch(peek(), c);
    read();
  }
  std::string val = "SIP/";
  // 1*DIGIT
  if (peek() >= '0' && peek() <= '9') {
    while (peek() >= '0' && peek() <= '9') {
      val += read();
    }
    // "."
    mustMatch(peek(), '.');
    val += read();

    // 1*DIGIT
    if (peek() >= '0' && peek() <= '9') {
      while (peek() >= '0' && peek() <= '9') {
        val += read();
      }
    } else {
      throw UnmatchException(cur_ptr_, "0-9");
    }
  } else {
    throw UnmatchException(cur_ptr_, "0-9");
  }
  return val;
}

// this is HUGE! we only implement those we will encounter.
MessageHeader SIPParser::MESSAGE_HEADER() {
  std::string header_name = readTill(HCOLON);
  mustMatch(peek(), HCOLON);
  read();
  eatSpaces();
  std::string header_parm = readTillCRLF();
  return MessageHeader(header_name, header_parm);
}

MessageHeader SIPParser::EXPECT_HEADER(const std::string &header_name) {
  for (char c : header_name) {
    mustMatch(peek(), c);
    read();
  }
  std::string headerName = header_name;
  mustMatch(peek(), HCOLON);
  read();
  eatSpaces();
  std::string headerParm = readTillCRLF();
  return MessageHeader(headerName, headerParm);
}

MessageHeader SIPParser::VIA() {
  // Via
  for (char c : std::string("Via")) {
    mustMatch(peek(), c);
    read();
  }
  std::string headerName = "Via";
  mustMatch(peek(), HCOLON);
  read();
  eatSpaces();
  std::string headerParm = readTillCRLF();
  return MessageHeader(headerName, headerParm);
}

// * 0x00-0xFF
// well... we use 0x01-0xff instead.
// since sdp wont have 0x00.
// and sdp is the only once we care.
std::string SIPParser::MESSAGE_BODY() { return readTillEnd(); }

std::string SIPParser::readTillEnd() { return data_.substr(cur_ptr_); }

std::string SIPParser::readTill(char c) {
  size_t tmp = cur_ptr_;
  while (peek() != c) {
    read();
  }
  if (tmp == cur_ptr_) {
    return "";
  }
  return data_.substr(tmp, cur_ptr_ - tmp);
}
