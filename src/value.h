#pragma once
#include <variant>

using IntegerT = int;
using LongT = long;

struct Value {
    std::variant<std::string, float, double, IntegerT, LongT> value;

    // Implement operator =()
};
