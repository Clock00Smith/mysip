#pragma once
#include <string>

class SDP
{
public:
    SDP(int port)
    {
        raw_ = "v=0\r\n"
               "o=MY 12345 12345 IN IP4 192.168.8.201\r\n"
               "s=MY\r\n"
               "c=IN IP4 192.168.8.201\r\n"
               "t=0 0\r\n"
               "m=audio " +
               std::to_string(port) + " RTP/AVP 0\r\n"
                                      "a=rtpmap:0 PCMU/8000\r\n";
    }
    std::string toString()
    {

        return raw_;
    }

private:
    std::string raw_;
    int port_;
};
