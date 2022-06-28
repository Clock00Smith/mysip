#pragma once
// We dont distinguish the different header for now.
class MessageHeader
{
public:
    MessageHeader(const std::string &name, const std::string &data) : name_(name), data_(data) {}
    bool operator==(const MessageHeader &other) const
    {
        return name_ == other.name_ && data_ == other.data_;
    }

    bool operator!=(const MessageHeader &other) const
    {
        return !(*this == other);
    }
    friend std::ostream &operator<<(std::ostream &os, const MessageHeader &obj)
    {
        return os << obj.name_ << ": " << obj.data_;
    }
    std::string name() const
    {
        return name_;
    }
    std::string data() const
    {
        return data_;
    }

private:
    std::string name_;
    std::string data_;
};
class SIPMessage
{
public:
    enum MessageType {
        RAW,
        REQUEST,
        RESPONSE
    };
    bool operator==(const SIPMessage &other) const
    {
        if (typeid(*this) != typeid(other))
        {
            return false;
        }
        return this->_equal(other);
    }
    virtual bool _equal(const SIPMessage &other) const = 0;
    virtual std::shared_ptr<SIPMessage> genReply(int code) = 0;
    virtual std::shared_ptr<SIPMessage> genReply(int code, const std::string &body) = 0;
    virtual std::ostream &_print(std::ostream &os) const = 0;
    virtual std::string toString() const = 0;
    virtual MessageType type() const
    {
        return MessageType::RAW;
    }
    friend std::ostream &operator<<(std::ostream &os, const SIPMessage &obj)
    {
        return obj._print(os);
    }
};
