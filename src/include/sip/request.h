#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include "sip-message.h"
#include "response.h"
#include <map>

class RequestURI
{
public:
    RequestURI(std::string data) { raw_ = data; }
    bool operator==(const RequestURI &other) const
    {
        return raw_ == other.raw_;
    }
    friend std::ostream &operator<<(std::ostream &os, const RequestURI &obj)
    {
        return os << obj.raw_;
    }

private:
    std::string raw_;
};

class RequestLine
{
public:
    RequestLine(std::string method, RequestURI uri, std::string sipVersion) : method_(method), uri_(uri), sipVersion_(sipVersion) {}
    std::string method() const
    {
        return method_;
    }
    bool operator==(const RequestLine &other) const
    {
        return method_ == other.method_ && sipVersion_ == other.sipVersion_ && uri_ == other.uri_;
    }
    friend std::ostream &operator<<(std::ostream &os, const RequestLine &obj)
    {
        return os << obj.method_ << " " << obj.uri_ << " " << obj.sipVersion_ << std::endl;
    }

private:
    std::string method_;
    RequestURI uri_;
    std::string sipVersion_;
};

class MessageBody
{
};

class Request : public SIPMessage
{
public:
    std::map<int, std::string> STATUS_DESC = {
        {200, "OK"},
        {100, "Trying"}};
    Request(RequestLine rl) : rl_(rl) {}
    void AddHeaders(MessageHeader mh)
    {
        headers_.push_back(mh);
    }
    bool operator==(const Request &other) const
    {
        if (rl_ == other.rl_ && headers_.size() == other.headers_.size())
        {
            for (size_t i = 0; i < headers_.size(); i++)
            {
                if (headers_[i] != other.headers_[i])
                {
                    std::cerr << "diff: " << i << std::endl;
                    std::cerr << headers_[i] << std::endl
                              << other.headers_[i] << std::endl;
                    return false;
                }
            }
            if (body_ == other.body_)
            {
                return true;
            }
            else
            {
                std::cerr << "body not equal." << std::endl;
                return false;
            }
        }
        return false;
    }
    std::string &body()
    {
        return body_;
    }
    std::string getMethod() const
    {
        return rl_.method();
    }
    MessageHeader getHeader(const std::string &name) const
    {

        for (auto itr = headers_.cbegin(); itr != headers_.cend(); itr++)
        {
            if (itr->name() == name)
            {
                return *itr;
            }
        }
    }
    virtual std::shared_ptr<SIPMessage> genReply(int code) override
    {
        std::vector<MessageHeader> headers;
        headers.push_back(getHeader("Via"));
        headers.push_back(getHeader("From"));
        headers.push_back(getHeader("To"));
        headers.push_back(getHeader("Call-ID"));
        headers.push_back(getHeader("CSeq"));
        switch (code)
        {
        case 100:
        {

            return std::make_shared<Response>(code, "Trying", headers);
        }
        case 200:
        {
            if (getMethod() == "INVITE")
            {
                std::string body = "v=0\r\n"
                                   "o=MY 12345 12345 IN IP4 192.168.8.201\r\n"
                                   "s=MY\r\n"
                                   "c=IN IP4 192.168.8.201\r\n"
                                   "t=0 0\r\n"
                                   "m=audio 10000 RTP/AVP 0\r\n"
                                   "a=rtpmap:0 PCMU/8000\r\n";
                return std::make_shared<Response>(code, "OK", headers, body);
            }
            else
            {
                return std::make_shared<Response>(code, "OK", headers);
            }
        }
        }
        if (STATUS_DESC.find(code) == STATUS_DESC.cend())
        {
            return std::make_shared<Response>(code, "UNKNOWN", headers_);
        }
        return std::make_shared<Response>(code, STATUS_DESC[code], headers_);
    }
    virtual bool _equal(const SIPMessage &other) const override
    {

        return this->operator==(dynamic_cast<const Request &>(other));
    }
    virtual std::ostream &_print(std::ostream &os) const override
    {
        return os << *this;
    }
    virtual std::string toString() const override
    {
        return "TODO";
    }
    virtual MessageType type() const override
    {
        return MessageType::REQUEST;
    }
    friend std::ostream &operator<<(std::ostream &os, const Request &obj)
    {
        os << std::endl
           << obj.rl_ << std::endl;
        for (auto itr = obj.headers_.cbegin(); itr != obj.headers_.cend(); itr++)
        {
            os << *itr << std::endl;
        }
        os << std::endl
           << obj.body_ << std::endl;
        return os;
    }

private:
    RequestLine rl_;
    std::vector<MessageHeader> headers_;
    std::string body_;
};