#pragma once
#include <endian.h>
#include <atomic>
#include <iostream>
#include <memory>
#include <vector>
#include "codecs/codec.h"
#include "socket/raw-socket.h"
class RTPSocket : public RawSocket {
 public:
  /*

      0                   1                   2                   3
      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |V=2|P|X|  CC   |M|     PT      |       sequence number         |
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |                           timestamp                           |
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |           synchronization source (SSRC) identifier            |
     +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
     |            contributing source (CSRC) identifiers             |
     |                             ....                              |
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

  */
#if BYTE_ORDER == LITTLE_ENDIAN
  struct RTPHeader {
    uint8_t CSRCCount : 4;
    uint8_t Extension : 1;
    uint8_t Padding : 1;
    uint8_t Version : 2;
    // 1st byte
    uint8_t PayloadType : 7;
    uint8_t Marker : 1;
    // 2nd byte
    uint16_t SequenceNumber;
    uint32_t Timestamp;
    uint32_t SSRC;
    // no csrc for now...
  };  //  12 bytes header.
#else
  struct RTPHeader {
    uint8_t Version : 2;
    uint8_t Padding : 1;
    uint8_t Extension : 1;
    uint8_t CSRCCount : 4;
    uint8_t Marker : 1;
    uint8_t PayloadType : 7;
    uint16_t SequenceNumber;
    uint32_t Timestamp;
    uint32_t SSRC;
  }
#endif
  RTPSocket(int port, std::string call_id, std::unique_ptr<Codec> codec);
  size_t Run();
  void Stop();

 private:
  uint16_t getSeqNo(const std::string &data) const;
  std::string getPayload(const std::string &data) const;
  std::string callId_;
  std::atomic<bool> running_;
  std::unique_ptr<Codec> codec_;
  std::vector<int> packetNumberRecv_;
};