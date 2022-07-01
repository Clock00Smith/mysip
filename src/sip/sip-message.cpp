#include "sip/sip-message.h"

bool SIPMessage::operator==(const SIPMessage &other) const {
    if (typeid(*this) != typeid(other)){
        return false;
    }
    return this->_equal(other);
}

SIPMessage::MessageType SIPMessage::type() const {
    return MessageType::RAW;
}

std::ostream &operator<<(std::ostream &os, const SIPMessage &obj){
    return obj._print(os);
}