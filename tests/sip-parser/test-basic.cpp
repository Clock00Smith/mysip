#include "sip/sip-parser.h"
#include "gtest/gtest.h"
TEST(TEST_INVITE, test_invite_basic)
{
    ASSERT_EQ(SIPParser("INVITE sip:bob@biloxi.example.com SIP/2.0\r\n").REQUEST_LINE(),
              RequestLine("INVITE", RequestURI("sip:bob@biloxi.example.com"), "SIP/2.0"));
}

TEST(TEST_INVITE, test_invite_with_single_header)
{
    Request req(SIPParser("INVITE sip:bob@biloxi.example.com SIP/2.0\r\n").REQUEST_LINE());
    req.AddHeaders(MessageHeader("Via", "SIP/2.0/TCP client.atlanta.example.com:5060;branch=z9hG4bK74bf9"));
    ASSERT_EQ(
        *(SIPParser("INVITE sip:bob@biloxi.example.com SIP/2.0\r\nVia: SIP/2.0/TCP client.atlanta.example.com:5060;branch=z9hG4bK74bf9\r\n\r\n").REQUEST()),
        req);
}

TEST(TEST_INVITE, test_inveite_with_multi_header)
{
    std::string raw = "INVITE sip:bob@biloxi.example.com SIP/2.0\r\n"
                      "Via: SIP/2.0/TCP client.atlanta.example.com:5060;branch=z9hG4bK74bf9\r\n"
                      "Max-Forwards: 70\r\n"
                      "From: Alice <sip:alice@atlanta.example.com>;tag=9fxced76sl\r\n"
                      "To: Bob <sip:bob@biloxi.example.com>\r\n"
                      "Call-ID: 3848276298220188511@atlanta.example.com\r\n"
                      "CSeq: 1 INVITE\r\n"
                      "Contact: <sip:alice@client.atlanta.example.com;transport=tcp>\r\n"
                      "Content-Type: application/sdp\r\n"
                      "Content-Length: 151\r\n\r\n";
    Request req(SIPParser("INVITE sip:bob@biloxi.example.com SIP/2.0\r\n").REQUEST_LINE());
    req.AddHeaders(MessageHeader("Via", "SIP/2.0/TCP client.atlanta.example.com:5060;branch=z9hG4bK74bf9"));
    req.AddHeaders(MessageHeader("Max-Forwards", "70"));
    req.AddHeaders(MessageHeader("From", "Alice <sip:alice@atlanta.example.com>;tag=9fxced76sl"));
    req.AddHeaders(MessageHeader("To", "Bob <sip:bob@biloxi.example.com>"));
    req.AddHeaders(MessageHeader("Call-ID", "3848276298220188511@atlanta.example.com"));
    req.AddHeaders(MessageHeader("CSeq", "1 INVITE"));
    req.AddHeaders(MessageHeader("Contact", "<sip:alice@client.atlanta.example.com;transport=tcp>"));
    req.AddHeaders(MessageHeader("Content-Type", "application/sdp"));
    req.AddHeaders(MessageHeader("Content-Length", "151"));
    ASSERT_EQ(*(SIPParser(raw).parse()), req);
}

TEST(TEST_INVITE, test_invite_with_sdp)
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
    auto req = SIPParser(raw_head).REQUEST();
    req->body() = raw_body;

    ASSERT_EQ(*req, *(SIPParser(raw_head + raw_body).REQUEST()));
}

TEST(TEST_INVITE, test_sipp_uac_invite)
{
    std::string head= "INVITE sip:service@192.168.8.201:5060 SIP/2.0\r\n"
                      "Via: SIP/2.0/UDP 192.168.8.201:5061;branch=z9hG4bK-951217-1-0\r\n"
                      "From: sipp <sip:sipp@192.168.8.201:5061>;tag=951217SIPpTag001\r\n"
                      "To: service <sip:service@192.168.8.201:5060>\r\n"
                      "Call-ID: 1-951217@192.168.8.201\r\n"
                      "CSeq: 1 INVITE\r\n"
                      "Contact: sip:sipp@192.168.8.201:5061\r\n"
                      "Max-Forwards: 70\r\n"
                      "Subject: Performance Test\r\n"
                      "Content-Type: application/sdp\r\n"
                      "Content-Length:   137\r\n"
                      "\r\n";
    std::string body = "v=0\r\n"
                       "o=user1 53655765 2353687637 IN IP4 192.168.8.201\r\n"
                       "s=-\r\n"
                       "c=IN IP4 192.168.8.201\r\n"
                       "t=0 0\r\n"
                       "m=audio 6000 RTP/AVP 0\r\n"
                       "a=rtpmap:0 PCMU/8000";
    ASSERT_NO_THROW(SIPParser(head + body).REQUEST());
}