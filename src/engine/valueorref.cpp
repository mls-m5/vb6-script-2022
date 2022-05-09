#include "valueorref.h"
#include "classinstance.h"

ValueOrRef::ValueOrRef(Value value)
    : value{std::move(value)} {}

ValueOrRef::ValueOrRef(Value *value)
    : value{value} {}

ValueOrRef::ValueOrRef(FunctionRef ref)
    : value{ref} {}

bool ValueOrRef::isFunction() {
    return value.index() == 2;
}

bool ValueOrRef::isArray() {
    if (value.index() == 0 || value.index() == 1) {
        if (get().typeName() == Type::Array) {
            return true;
        }
    }
    return false;
}

Value &ValueOrRef::get() {
    if (value.index() == 0) {
        return std::get<0>(value);
    }
    else {
        return *std::get<1>(value);
    }
}

const Value &ValueOrRef::get() const {
    if (value.index() == 0) {
        return std::get<0>(value);
    }
    else {
        return *std::get<1>(value);
    }
}

const FunctionRef &ValueOrRef::function() {
    if (value.index() != 2) {
        throw VBRuntimeError{"expression is not a function"};
    }
    return std::get<FunctionRef>(value);
}

Value *ValueOrRef::operator->() {
    return &get();
}

const Value *ValueOrRef::operator->() const {
    return &get();
}

Value &ValueOrRef::operator*() {
    return get();
}

const Value &ValueOrRef::operator*() const {
    return get();
}
