#pragma once

#include "value.h"

struct ValueOrRef {
private:
    std::variant<Value, Value *> value;

public:
    ValueOrRef() = delete;
    ValueOrRef(const ValueOrRef &) = default;
    ValueOrRef &operator=(const ValueOrRef &) = default;
    ValueOrRef(ValueOrRef &&) = default;
    ValueOrRef &operator=(ValueOrRef &&) = default;

    ValueOrRef(Value value)
        : value{std::move(value)} {}
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

    Value *operator->() {
        return &get();
    }

    const Value *operator->() const {
        return &get();
    }

    Value &operator*() {
        return get();
    }

    const Value &operator*() const {
        return get();
    }
};
