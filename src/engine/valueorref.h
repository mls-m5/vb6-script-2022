#pragma once

#include "value.h"
#include "vbruntimeerror.h"

struct FunctionRef {
    const class Function *function;
    ClassT me;

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

    ValueOrRef(Value value);
    ValueOrRef(Value *value);
    ValueOrRef(FunctionRef ref);

    Value &get();

    const Value &get() const;

    const FunctionRef &function();

    Value *operator->();

    const Value *operator->() const;

    Value &operator*();

    const Value &operator*() const;
};
