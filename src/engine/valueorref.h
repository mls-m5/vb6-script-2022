#pragma once

#include "value.h"

struct ValueOrRef {
    std::variant<Value, Value *> value;

    ValueOrRef() = default;
    ValueOrRef(const ValueOrRef &) = default;
    ValueOrRef &operator=(const ValueOrRef &) = default;
    ValueOrRef(ValueOrRef &&) = default;
    ValueOrRef &operator=(ValueOrRef &&) = default;

    ValueOrRef(const Value &value)
        : value{value} {}

    ValueOrRef(Value *value)
        : value{value} {}

    Value &get() {
        if (value.index() == 0) {
            return std::get<0>(value);
        }
        else {
            return *std::get<1>(value);
        }
    }

    const Value &get() const {
        if (value.index() == 0) {
            return std::get<0>(value);
        }
        else {
            return *std::get<1>(value);
        }
    }
};
