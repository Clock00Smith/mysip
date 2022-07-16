#include "sip/status-line.h"

StatusLine::StatusLine(std::string sipVersion, int statusCode, std::string reasonPhrase)
    : sipVersion_(std::move(sipVersion)), statusCode_(statusCode), reasonPhrase_(std::move(reasonPhrase)) {}

int StatusLine::statusCode() const { return statusCode_; }

std::string StatusLine::reasonPhrase() const { return reasonPhrase_; }