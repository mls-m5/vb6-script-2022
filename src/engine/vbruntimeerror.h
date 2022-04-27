#pragma once

#include <stdexcept>

class VBRuntimeError : std::runtime_error {
public:
    VBRuntimeError(const std::string str)
        : std::runtime_error{str} {}
};
