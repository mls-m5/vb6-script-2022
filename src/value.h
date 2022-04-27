#pragma once
#include "type.h"
#include <memory>
#include <string>

struct Value {
    std::variant<std::string,
                 float,
                 double,
                 IntegerT,
                 LongT,
                 std::shared_ptr<ClassInstance>,
                 std::unique_ptr<ClassInstance>>
        value;

    Value() = default;
    Value(Value &&) = default;
    Value(const Value &) = delete;
    Value &operator=(Value &&) = default;
    Value &operator=(const Value &) = delete;
    ~Value() = default;

    Value(IntegerT i)
        : value{i} {}

    // Implement operator =()
};
