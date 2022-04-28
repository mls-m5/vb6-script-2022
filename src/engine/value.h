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

    Value(double d)
        : value{d} {}

    Value(LongT l)
        : value{l} {}

    Value(std::shared_ptr<ClassInstance> i)
        : value{std::move(i)} {}

    Value(std::unique_ptr<ClassInstance> i)
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

    Type::TypeName type() const {
        return static_cast<Type::TypeName>(value.index());
    }

    std::string toString() const;

    // Set value and change type at the same time
    //    void setAny(const Value &other) {
    //        value = other.value;
    //    }

    // Implement operator =()
};
