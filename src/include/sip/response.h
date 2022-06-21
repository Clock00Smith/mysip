#pragma once
#include "sip-message.h"

class Response : public SIPMessage {
public:
    virtual bool _equal(const SIPMessage& other) const override {
        return false; 
    }
    virtual std::ostream& _print(std::ostream& os) const override {return os;}
};