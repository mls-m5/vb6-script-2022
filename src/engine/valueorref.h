#pragma once

#include "value.h"

struct ValueOrRef {
    std::variant<Value, Value *> value;

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
