#pragma once

#include "value.h"
#include <vector>

struct FunctionArgumentValue {
    std::variant<Value, Value *> value;

    Value &get() {
        if (value.index()) {
            return *std::get<Value *>(value);
        }
        else {
            return std::get<Value>(value);
        }
    }

    const Value &get() const {
        if (value.index()) {
            return *std::get<Value *>(value);
        }
        else {
            return std::get<Value>(value);
        }
    }
};

// First argument is "this"-argument
using FunctionArgumentValues = std::vector<FunctionArgumentValue>;
