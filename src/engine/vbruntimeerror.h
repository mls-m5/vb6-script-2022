#pragma once

#include "Location.h"
#include <stdexcept>

class VBRuntimeError : std::runtime_error {
public:
    VBRuntimeError(const std::string str)
        : std::runtime_error{str} {}

    VBRuntimeError(const Location &location, const std::string str)
        : std::runtime_error{location.toString() + ": " + str} {}
};
