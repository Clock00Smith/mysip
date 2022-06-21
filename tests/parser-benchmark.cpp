#include "sip/sip-parser.h"
#include <ctime>
constexpr int TIMES = 1000000;
int main()
{
    std::string raw_head = "INVITE sip:bob@biloxi.example.com SIP/2.0\r\n"
                           "Via: SIP/2.0/TCP client.atlanta.example.com:5060;branch=z9hG4bK74bf9\r\n"
                           "Max-Forwards: 70\r\n"
                           "From: Alice <sip:alice@atlanta.example.com>;tag=9fxced76sl\r\n"
                           "To: Bob <sip:bob@biloxi.example.com>\r\n"
                           "Call-ID: 3848276298220188511@atlanta.example.com\r\n"
                           "CSeq: 1 INVITE\r\n"
                           "Contact: <sip:alice@client.atlanta.example.com;transport=tcp>\r\n"
                           "Content-Type: application/sdp\r\n"
                           "Content-Length: 151\r\n"
                           "\r\n";
    std::string raw_body = "v=0\r\n"
                           "o=alice  2890844526  2890844526  IN IP4 client.atlanta.example.com\r\n"
                           "s=-\r\n"
                           "c=IN  IP4  192.0.2.101\r\n"
                           "t=0 0\r\n"
                           "m=audio  49172  RTP/AVP 0\r\n"
                           "a=rtpmap:0  PCMU/8000";
    std::string data = raw_head + raw_body;
    std::time_t start = std::time(nullptr);
    for (int i = 0; i < TIMES; i++)
    {
        SIPParser(data).REQUEST();
    }
    std::time_t end = std::time(nullptr);
    std::cout << end - start << std::endl;
    std::cout << TIMES / (end - start) << " request per sec." << std::endl;
    std::cout << SIPParser(data).REQUEST(); 
}