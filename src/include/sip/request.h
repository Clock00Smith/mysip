#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include "sip-message.h"

class RequestURI
{
public:
    RequestURI(std::string data) {raw_ = data;}
    bool operator==(const RequestURI& other) const {
        return raw_ == other.raw_;
    }
    friend std::ostream& operator<<(std::ostream& os, const RequestURI& obj){
        return os << obj.raw_;
    }
private:
    std::string raw_;
};

class RequestLine
{
public:
    RequestLine(std::string method, RequestURI uri, std::string sipVersion): method_(method), uri_(uri), sipVersion_(sipVersion){}
    bool operator==(const RequestLine& other) const {
        return method_ == other.method_ && sipVersion_ == other.sipVersion_ && uri_ == other.uri_;
    }
    friend std::ostream& operator<<(std::ostream& os, const RequestLine& obj){
        return os << obj.method_ << " " << obj.uri_ << " " << obj.sipVersion_ << std::endl;
    }
private:
    std::string method_;
    RequestURI uri_;
    std::string sipVersion_;
};

// We dont distinguish the different header for now.
class MessageHeader
{
    public:
    MessageHeader(const std::string& name, const std::string& data) : name_(name), data_(data){}
    bool operator==(const MessageHeader& other) const {
        return name_ == other.name_ && data_ == other.data_;
    }

    bool operator!=(const MessageHeader& other) const {
        return !(*this == other);
    }
    friend std::ostream& operator<<(std::ostream& os, const MessageHeader& obj){
        return os << obj.name_ << ": " << obj.data_;
    }
    private:
    std::string name_;
    std::string data_;
};

class MessageBody
{
};

class Request : public SIPMessage
{
    public:
        Request(RequestLine rl): rl_(rl) {}
        void AddHeaders(MessageHeader mh) {
            headers_.push_back(mh);
        }
        bool operator==(const Request& other) const {
            if (rl_ == other.rl_ && headers_.size() == other.headers_.size()){
                for (size_t i = 0; i < headers_.size(); i++){
                    if (headers_[i] != other.headers_[i]){
                        std::cerr << "diff: " << i << std::endl;
                        std::cerr << headers_[i] << std::endl << other.headers_[i] << std::endl;
                        return false;
                    }
                }
                if (body_ == other.body_){
                    return true;
                } else {
                    std::cerr << "body not equal." << std::endl;
                    return false;
                }
            }
            return false;
        }
        std::string& body() {
            return body_;
        }
        virtual bool _equal(const SIPMessage& other) const override {

            return this->operator==(dynamic_cast<const Request&>(other));
        }
        virtual std::ostream& _print(std::ostream& os) const override{
            return os << *this;
        }
        friend std::ostream& operator<<(std::ostream& os, const Request& obj) {
            os << std::endl << obj.rl_ << std::endl;
            for (auto itr = obj.headers_.cbegin(); itr != obj.headers_.cend(); itr++){
                os << *itr << std::endl;
            }
            os << std::endl << obj.body_ << std::endl;
            return os;
        }

    private:
    RequestLine rl_;
    std::vector<MessageHeader> headers_;
    std::string body_;
};