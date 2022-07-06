#pragma once
#include <string>
#include <atomic>
#include <mutex>
class SDP
{
public:
    static SDP GetSDPWithCodec(const std::string &codec, const std::string &call_id);
    std::string toString() const ;

private:
    static int nextPort;
    static std::mutex portLock;
    std::string raw_;
    int port_;
    SDP(int port);
};
