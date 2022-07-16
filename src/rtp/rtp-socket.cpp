#include "rtp/rtp-socket.h"

RTPSocket::RTPSocket(int port, std::string call_id, std::unique_ptr<Codec> codec)
    : RawSocket("", port), callId_(std::move(call_id)), running_(true), codec_(std::move(codec)){};

size_t RTPSocket::Run() {
  size_t count = 0;
  uint16_t last = 0;
  uint16_t cur = 0;
  std::cout << "RTPSocket running: " << port() << std::endl;
  while (running_) {
    RecvData rd = Recv();  // this will block, so we will never know when to close it.
    if (rd.data.empty()) {
      continue;
    }
    count++;
    cur = getSeqNo(rd.data);
    if (cur < last && cur != 0) {
      std::cout << "maybe a jitter " << std::endl;
    }
    last = cur;
    if (rd.data.size() <= 12) {
    } else {
      codec_->decode(getPayload(rd.data));
    }
  }
  std::cout << "RTPSocket end: " << count << " pack recv." << std::endl;
  return count;
}

void RTPSocket::Stop() { running_ = false; }

uint16_t RTPSocket::getSeqNo(const std::string &data) const {
  const struct RTPHeader *header = reinterpret_cast<const RTPHeader *>((data.substr(0, 12)).c_str());
  return ntohs(header->SequenceNumber);
}

std::string RTPSocket::getPayload(const std::string &data) const {
  if (data.size() <= 12) {
    return "";
  }
  return data.substr(12);
}