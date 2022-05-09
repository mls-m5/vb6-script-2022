#pragma once
#include "type.h"
#include <memory>
#include <string>

struct Value {
    TypeVariantT value;

    Value() = default;
    Value(Value &&) = default;
    Value(const Value &) = default;

    // TODO: Do so these functions refuses to change type
    Value &operator=(Value &&);
    Value &operator=(const Value &);
    ~Value() = default;

    bool operator==(const Value &other) const;

    Value(IntegerT i);

    Value(std::string str);

    Value(SingleT d);

    Value(DoubleT d);

    Value(LongT l);

    Value(ClassT i);

    Value(StructT i);

    Value(ArrayT array);

    //! Set operation that also changes the type of the value
    void forceSet(Value &other);

    static Value create(Type type);

    template <typename T>
    T &get() {
        return std::get<T>(value);
    }

    template <typename T>
    const T &get() const {
        return std::get<T>(value);
    }

    //! Set and ignore value test
    void forceSet(Value value);

    Type::TypeName typeName() const;

    Type type() const;

    std::string toString() const;

    LongT toInteger() const;

    DoubleT toFloat() const;

    bool isNumber() const;

    bool toBool() const;

    ClassT toClass() const;

    // What type binary operators will result in
    Type commonType(Type other) const;

    template <typename T>
    T to() const {
        if constexpr (std::is_same_v<IntegerT, T> || std::is_same_v<LongT, T>) {
            return toInteger();
        }
        else if (std::is_same_v<SingleT, T> || std::is_same_v<DoubleT, T>) {
            return toFloat();
        }
    }

    // Flip sign value
    Value negative() const;

    // Implement operator =()
};
