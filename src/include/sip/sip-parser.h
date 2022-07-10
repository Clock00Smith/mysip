#pragma once
#include <string>
#include <iostream>
#include <exception>
#include <memory>
#include "request.h"
#include "response.h"

constexpr char SPACE = ' ';
constexpr char CR = '\r';
constexpr char LF = '\n';
constexpr char HCOLON = ':';
class SIPParser
{

public:
    class UnmatchException : public std::runtime_error
    {
    public:
        UnmatchException(size_t cur_pos_, std::string expect) : std::runtime_error("Unmatch at: " + std::to_string(cur_pos_) + " expecting: " + expect){};
    };
    class UnimplementException : public std::runtime_error
    {
    public:
        UnimplementException() : std::runtime_error("Unimplemented") {}
    };
    SIPParser(const std::string &data);
    std::string read();
    char peek();
    char peek(size_t n);
    bool match(char c, char expect);
    void mustMatch(char c, char expect);
    std::string readTillCRLF();
    void eatSpaces();
    std::shared_ptr<SIPMessage> parse();
    bool IsRequest() const;
    // Request / Response
    std::shared_ptr<SIPMessage> SIP_MESSAGE();
    bool IsMethod();
    void CRLF();
    void SP();
    // Repuest-Line *(message-header) CRLF [message-body]
    std::shared_ptr<Request> REQUEST();
    std::shared_ptr<Response> RESPONSE();
    RequestLine REQUEST_LINE();

    std::string REQUEST_URI();
    std::string INVITE();
    std::string ACK();
    std::string BYE();
    std::string CANCEL();
    std::string METHOD();
    // SIP-Version = "SIP" "/" 1*DIGIT "." 1*DIGIT
    std::string SIP_VERSION();
    // this is HUGE! we only implement those we will encounter.
    MessageHeader MESSAGE_HEADER();

    MessageHeader EXPECT_HEADER(const std::string &header_name);
    MessageHeader VIA();
    // * 0x00-0xFF
    // well... we use 0x01-0xff instead.
    // since sdp wont have 0x00.
    // and sdp is the only once we care.
    std::string MESSAGE_BODY();
    std::string readTillEnd();
    std::string readTill(char c);

private:
    std::string data_;
    size_t cur_ptr_;
};