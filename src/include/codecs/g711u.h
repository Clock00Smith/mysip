#pragma once
#include <fstream>
#include "codec.h"
#include <string>
class G711U : public Codec
{
public:
    G711U(const std::string &path)
    {
        file_.open(path);
    }
    virtual std::string decode(const std::string &data) override
    {

        std::string val = "";
        for (char c : data)
        {
            const uint16_t MULAW_BIAS = 33;
            uint8_t sign = 0, position = 0;
            int16_t decoded = 0;
            int8_t number = ~(int8_t)c;
            if (number & 0x80)
            {
                number &= ~(1 << 7);
                sign = -1;
            }
            position = ((number & 0xF0) >> 4) + 5;
            decoded = ((1 << position) | ((number & 0x0F) << (position - 4)) | (1 << (position - 5))) - MULAW_BIAS;
            if (sign != 0)
            {
            }
            else
            {
                decoded = -decoded;
            }

            file_.write((const char *)&decoded, 2);
        }
        file_.flush();
        return "";
    }
    virtual std::string encode(const std::string &data) override
    {
        return "";
    }

private:
    std::ofstream file_;
};