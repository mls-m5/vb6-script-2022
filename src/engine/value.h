#pragma once
#include "type.h"
#include <memory>
#include <string>

struct Value {
    TypeVariantT value;

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

    LongT toInteger() const;

    DoubleT toFloat() const;

    bool toBool() const;

    ClassT toClass() const;

    // Implement operator =()
};
