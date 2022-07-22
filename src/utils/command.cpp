#include "utils/command.h"
#include <iostream>
// command = comm *(SP params)
Command::Command(std::string input) {
  size_t ptr = 0;
  if (input.empty()) {
    command_ = "unknown";
  }
  switch (input[0]) {
    case 'q':
      command_ = "quit";
      break;
    case 'i':
      command_ = "invite";
      if (input.size() < 2) {
        params_ = "";
      } else {
        params_ = input.substr(2);
      }
      break;
    default:
      break;
  }
}

const std::string Command::command() const {
    return command_;
}

const std::string Command::params() const {
    return params_;
}