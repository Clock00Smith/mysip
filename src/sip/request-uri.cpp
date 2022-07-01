#include "sip/request-uri.h"

RequestURI::RequestURI(std::string data) : raw_(data)
{
}

bool RequestURI::operator==(const RequestURI &other) const {
    return raw_ == other.raw_;
}

std::ostream &operator<<(std::ostream &os, const RequestURI &obj){
    return os << obj.raw_;
}

