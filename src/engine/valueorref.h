#pragma once

#include "value.h"

struct FunctionRef {
    const class Function *function;

    const Function *get() {
        return function;
    }

    const Function *get() const {
        return function;
    }
};

struct ValueOrRef {
private:
    std::variant<Value, Value *, FunctionRef> value;

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
    ValueOrRef(FunctionRef ref)
        : value{ref} {}

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

    const FunctionRef &function() {
        return std::get<FunctionRef>(value);
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
