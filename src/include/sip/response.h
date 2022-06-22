#pragma once
#include "sip-message.h"

class Response : public SIPMessage
{
public:
    Response(int code, const std::string &status, std::vector<MessageHeader> headers) : statusCode_(code), statusDesc_(status), headers_(headers) {
        headers_.push_back(MessageHeader("Content-Length", "0"));
    }
    Response(int code, const std::string &status, std::vector<MessageHeader> headers, const std::string &body) : statusCode_(code), statusDesc_(status), headers_(headers), body_(body)
    {
        headers_.push_back(MessageHeader("Content-Length", std::to_string(body.size())));
        headers_.push_back(MessageHeader("Content-Type", "application/sdp"));
    }
    int code() const
    {
        return statusCode_;
    }
    virtual bool _equal(const SIPMessage &other) const override
    {
        return false;
    }
    virtual std::string toString() const override
    {
        std::string val = "";
        val += "SIP/2.0 " + std::to_string(statusCode_) + " " + statusDesc_ + "\r\n";
        for (auto itr = headers_.cbegin(); itr != headers_.cend(); itr++)
        {
            val += itr->name() + ": " + itr->data() + "\r\n";
        }
        val += "\r\n";
        val += body_ + "\r\n";
        return val;
    }
    virtual MessageType type() const
    {
        return MessageType::RESPONSE;
    }
    virtual std::ostream &_print(std::ostream &os) const override { return os << toString(); }
    virtual std::shared_ptr<SIPMessage> genReply(int code) override
    {
        return std::make_shared<Response>(100, "Trying", std::vector<MessageHeader>());
    }

private:
    int statusCode_;
    std::string statusDesc_;
    std::vector<MessageHeader> headers_;
    std::string body_;
};