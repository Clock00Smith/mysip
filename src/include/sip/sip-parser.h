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
    SIPParser(const std::string &data) : data_(data), cur_ptr_(0) {}
    std::string read()
    {
        size_t tmp = cur_ptr_;
        cur_ptr_++;
        return std::string(data_.substr(tmp, 1));
    }
    char peek()
    {
        if (cur_ptr_ >= data_.size())
        {
            return '\0';
        }
        return data_[cur_ptr_];
    }
    char peek(size_t n)
    {
        if (cur_ptr_ + n - 1 >= data_.size())
        {
            return '\0';
        }
        return data_[cur_ptr_ + n - 1];
    }
    bool match(char c, char expect)
    {
        return c == expect;
    }
    void mustMatch(char c, char expect)
    {
        if (cur_ptr_ >= data_.size())
        {
            throw UnmatchException(cur_ptr_, "");
        }
        else
        {
            if (data_[cur_ptr_] != expect)
            {
                throw UnmatchException(cur_ptr_, std::to_string(expect));
            }
        }
    }
    std::string readTillCRLF()
    {
        std::string val = "";
        while (peek() != '\r' && peek(2) != '\n')
        {
            val += read();
        }
        return val;
    }
    void eatSpaces()
    {
        while (peek() == SPACE)
        {
            read();
        }
    }
    std::shared_ptr<SIPMessage> parse()
    {
        return SIP_MESSAGE();
    }
    bool IsRequest() const
    {
        return false;
    }
    // Request / Response
    std::shared_ptr<SIPMessage> SIP_MESSAGE()
    {
        if (peek() == 'S' && peek(2) == 'I' && peek(3) == 'P')
        {
            return RESPONSE();
        }
        else
        {
            return REQUEST();
        }
    };
    bool IsMethod()
    {
        return true;
    }
    void CRLF()
    {
        mustMatch(peek(), CR);
        read();
        mustMatch(peek(), LF);
        read();
    }
    void SP()
    {
        mustMatch(peek(), SPACE);
        read();
    }
    // Repuest-Line *(message-header) CRLF [message-body]
    std::shared_ptr<Request> REQUEST()
    {
        if (IsMethod())
        {
            RequestLine rl = REQUEST_LINE();
            Request req(rl);
            while (peek() != '\r')
            {
                MessageHeader mh = MESSAGE_HEADER();
                req.AddHeaders(mh);
                CRLF();
            }

            CRLF();
            req.body() = MESSAGE_BODY();
            return std::make_shared<Request>(req);
        }
    }
    std::shared_ptr<Response> RESPONSE()
    {
        return std::make_shared<Response>(Response());
    }
    RequestLine REQUEST_LINE()
    {
        std::string method = METHOD();
        SP();
        RequestURI ruri = REQUEST_URI();
        SP();
        std::string sip_version = SIP_VERSION();
        CRLF();
        return RequestLine(method, ruri, sip_version);
    }

    std::string REQUEST_URI()
    {
        std::string val = "";
        // TODO
        while (peek() != SPACE)
        {
            if (peek() == '\0')
            {
                break;
            }
            val += read();
        }
        return val;
    }
    std::string INVITE()
    {
        for (char c : std::string("INVITE"))
        {
            mustMatch(peek(), c);
            read();
        }
        return "INVITE";
    }
    std::string METHOD()
    {
        switch (peek())
        {
        case 'I':
            return INVITE();
        default:
            return "";
        }
    }
    // SIP-Version = "SIP" "/" 1*DIGIT "." 1*DIGIT
    std::string SIP_VERSION()
    {
        // "SIP" "/"
        for (char c : std::string("SIP/"))
        {
            mustMatch(peek(), c);
            read();
        }
        std::string val = "SIP/";
        // 1*DIGIT
        if (peek() >= '0' && peek() <= '9')
        {
            while (peek() >= '0' && peek() <= '9')
            {
                val += read();
            }
            // "."
            mustMatch(peek(), '.');
            val += read();

            // 1*DIGIT
            if (peek() >= '0' && peek() <= '9')
            {
                while (peek() >= '0' && peek() <= '9')
                {
                    val += read();
                }
            }
            else
            {
                throw UnmatchException(cur_ptr_, "0-9");
            }
        }
        else
        {
            throw UnmatchException(cur_ptr_, "0-9");
        }
        return val;
    }
    // this is HUGE! we only implement those we will encounter.
    MessageHeader MESSAGE_HEADER()
    {
        std::string header_name = readTill(HCOLON);
        mustMatch(peek(), HCOLON);
        read();
        eatSpaces();
        std::string header_parm = readTillCRLF();
        return MessageHeader(header_name, header_parm);
    }

    MessageHeader EXPECT_HEADER(const std::string &header_name)
    {
        for (char c : header_name)
        {
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
    MessageHeader VIA()
    {
        // Via
        for (char c : std::string("Via"))
        {
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
    std::string MESSAGE_BODY()
    {
        return readTillEnd();
    }
    std::string readTillEnd()
    {
        return data_.substr(cur_ptr_);
    }
    std::string readTill(char c)
    {
        size_t tmp = cur_ptr_;
        while (peek() != c)
        {
            read();
        }
        if (tmp == cur_ptr_)
        {
            return "";
        }
        return data_.substr(tmp, cur_ptr_ - tmp);
    }

private:
    std::string data_;
    size_t cur_ptr_;
};