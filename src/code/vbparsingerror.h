#pragma once

#include "token.h"
#include <stdexcept>

struct VBParsingError : std::runtime_error {
    VBParsingError(const Token &, std::string text)
        : std::runtime_error{text} {}
};
