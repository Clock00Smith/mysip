#include "sip/status-line.h"

StatusLine::StatusLine(const std::string &sipVersion, int statusCode, const std::string &reasonPhrase)
    : sipVersion_(sipVersion), statusCode_(statusCode), reasonPhrase_(reasonPhrase) {}

int StatusLine::statusCode() const { return statusCode_; }

std::string StatusLine::reasonPhrase() const { return reasonPhrase_; }