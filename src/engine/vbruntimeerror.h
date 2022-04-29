#pragma once

#include "Location.h"
#include <stdexcept>

class VBRuntimeError : std::runtime_error {
public:
    VBRuntimeError(const Location &location, const std::string str)
        : std::runtime_error{location.path->string() + ":" +
                             std::to_string(location.line) + str} {}

    VBRuntimeError(const std::string str)
        : std::runtime_error{str} {}
};
