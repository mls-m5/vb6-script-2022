#pragma once

#include "token.h"
#include <stdexcept>

struct VBParsingError : std::runtime_error {
    VBParsingError(const Location &loc, std::string text)
        : std::runtime_error{loc.path->string() + ": " +
                             std::to_string(loc.line) + ": " + text} {}
};

// Should never be encountered by user
struct VBInternalParsingError : std::runtime_error {
    VBInternalParsingError(const Location &loc, std::string text)
        : std::runtime_error{loc.path->string() + ": " +
                             std::to_string(loc.line) + " " + text} {}
};
