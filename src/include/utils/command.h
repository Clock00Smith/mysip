#pragma once
#include <string>
#include <vector>
class Command {
public:
    Command(std::string input);
    const std::string command() const;
    const std::string params() const;
private:
    std::string command_;
    std::string params_;
};