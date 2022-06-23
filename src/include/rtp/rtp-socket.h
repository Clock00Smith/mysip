#pragma once
#include "socket/raw-socket.h"
#include "codecs/codec.h"
#include <atomic>
#include <memory>
#include <vector>
#include <iostream>
class RTPSocket : public RawSocket
{
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
    struct RTPHeader
    {
        // shit! we are little endian!
        uint8_t CSRCCount : 4;
        uint8_t Extension : 1;
        uint8_t Padding : 1;
        uint8_t Version : 2;
        uint8_t PayloadType : 7;
        uint8_t Marker : 1;
        uint16_t SequenceNumber;
        uint32_t Timestamp;
        uint32_t SSRC;
        // no csrc for now...
    }; //  12 bytes header.
    RTPSocket(int port,
              const std::string &call_id,
              std::unique_ptr<Codec> codec)
        : RawSocket("", port),
          callId_(call_id),
          running_(true),
          codec_(std::move(codec))
    {
    }
    size_t Run()
    {
        size_t count = 0;
        uint16_t last = 0;
        uint16_t cur = 0;
        while (running_)
        {
            RecvData rd = Recv();
            count++;
            cur = getSeqNo(rd.data);
            if (cur < last) {
                std::cout << "maybe a jitter " << std::endl;
            }
            last = cur;
            std::cout << "seqNo: " << cur << " total: " << count << std::endl;
            codec_->decode(getPayload(rd.data));
        }
    }
    void Stop()
    {
        running_ = false;
    }

private:
    uint16_t getSeqNo(const std::string &data) const
    {
        const struct RTPHeader *header = reinterpret_cast<const RTPHeader *>((data.substr(0, 12)).c_str());
        return ntohs(header->SequenceNumber);
    }
    std::string getPayload(const std::string &data) const
    {
        std::cout << data.substr(12).size() << std::endl; 
        return data.substr(12);

    }
    std::string callId_;
    std::atomic<bool> running_;
    std::unique_ptr<Codec> codec_;
    std::vector<int> packetNumberRecv_;
};