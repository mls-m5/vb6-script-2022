#pragma once

#include "token.h"
#include <stdexcept>

struct VBParsingError : std::runtime_error {
    VBParsingError(const Location &, std::string text)
        : std::runtime_error{text} {}
};
