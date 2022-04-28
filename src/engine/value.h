#pragma once
#include "type.h"
#include <memory>
#include <string>

struct Value {
    std::
        variant<std::string, SingleT, DoubleT, IntegerT, LongT, ClassT, StructT>
            value;

    Value() = default;
    Value(Value &&) = default;
    Value(const Value &) = default;
    Value &operator=(Value &&) = default;
    Value &operator=(const Value &) = default;
    ~Value() = default;

    Value(IntegerT i)
        : value{i} {}

    Value(std::string str)
        : value{std::move(str)} {}

    Value(SingleT d)
        : value{d} {}

    Value(DoubleT d)
        : value{d} {}

    Value(LongT l)
        : value{l} {}

    Value(ClassT i)
        : value{std::move(i)} {}

    Value(StructT i)
        : value{std::move(i)} {}

    static Value create(Type type);

    template <typename T>
    auto &get() {
        return std::get<T>(value);
    }

    template <typename T>
    auto &get() const {
        return std::get<T>(value);
    }

    Type::TypeName typeName() const {
        return static_cast<Type::TypeName>(value.index());
    }

    Type type() const;

    std::string toString() const;

    // Set value and change type at the same time
    //    void setAny(const Value &other) {
    //        value = other.value;
    //    }

    // Implement operator =()
};
