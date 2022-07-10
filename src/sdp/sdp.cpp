#include <codecs/g711u.h>
#include <rtp/rtp-socket.h>
#include <sdp/sdp.h>
int SDP::nextPort = 10000;
std::mutex SDP::portLock;
std::string SDP::toString() const { return raw_; }

// this will choose the port, and try to bind to it,
// if success, return a SDP
SDP SDP::GetSDPWithCodec(const std::string &codec, const std::string &call_id) {
  int round = 0;
  int port;
  do {
    {
      std::scoped_lock<std::mutex> lock(SDP::portLock);
      port = nextPort;
      nextPort += 2;
    }
    try {
      // TODO, make socket run;
      RTPSocket socket(port, call_id, std::make_unique<G711U>("./test-" + std::to_string(port) + ".pcm"));
    } catch (RawSocket::SocketException) {
      continue;
    }
    return SDP(port);
  } while (round < 50);
}

SDP::SDP(int port) {
  raw_ =
      "v=0\n"
      "o=MySIP 12345 12345 IN IP4 192.168.56.101\n"
      "s=MySIP\n"
      "c=IN IP4 192.168.56.101\n"
      "t=0 0\n"
      "m=audio " +
      std::to_string(port) +
      " RTP/AVP 0\n"
      "a=rtpmap:0 PCMU/8000\n\r\n";
}