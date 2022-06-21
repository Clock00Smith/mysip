#pragma once

class SIPMessage{
    public:
        bool operator==(const SIPMessage& other) const {
            if (typeid(*this) != typeid(other)){
                return false;
            }
            return this->_equal(other);
        }
        virtual bool _equal(const SIPMessage& other) const = 0;
        virtual std::ostream& _print(std::ostream& os) const = 0;
        friend std::ostream& operator<<(std::ostream& os, const SIPMessage& obj){
            return obj._print(os);
        }
};
