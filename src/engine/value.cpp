#include "value.h"
#include "classinstance.h"

bool Value::operator==(const Value &other) const {
    return toInteger() == other.toInteger();
}

Value Value::create(Type type) {
    switch (type.type) {
    case Type::String:
        return Value{std::string{}};
    case Type::Single:
        return Value{SingleT{0}};
    case Type::Double:
        return Value{DoubleT{0}};
    case Type::Long:
        return Value{LongT{0}};
    case Type::Integer:
        return Value{IntegerT{0}};
    case Type::Byte:
        return Value{ByteT{0}};
    case Type::Class:
        return Value{ClassInstance::create(type.classType)};
    case Type::Struct:
        return Value{StructT{type.classType}};
    }

    return {};
}

Type Value::type() const {
    auto type = typeName();
    ClassType *classType = nullptr;

    if (type == Type::Struct) {
        classType = get<StructT>().get()->type();
    }
    else if (type == Type::Class) {
        classType = get<ClassT>().get()->type();
    }

    return {type, classType};
}

std::string Value::toString() const {
    switch (typeName()) {
    case Type::String:
        return get<std::string>();
    case Type::Single:
        return std::to_string(get<SingleT>());
    case Type::Double:
        return std::to_string(get<DoubleT>());
    case Type::Long:
        return std::to_string(get<LongT>());
    case Type::Integer:
        return std::to_string(get<IntegerT>());
    case Type::Byte:
        return std::to_string(get<ByteT>());
    case Type::Class:
        throw VBRuntimeError{"could not convert class to string"};
    case Type::Struct:
        throw VBRuntimeError{"could not convert struct to string"};
    }

    return {};
}

LongT Value::toInteger() const {
    switch (typeName()) {
    case Type::String:
        return std::stol(get<std::string>());
    case Type::Single:
        return get<SingleT>() + .5;
    case Type::Double:;

        return get<DoubleT>() + .5;
    case Type::Long:
        return get<LongT>();
    case Type::Integer:
        return get<IntegerT>();
    case Type::Byte:
        return get<ByteT>();
    case Type::Class:
        throw VBRuntimeError{"could not convert class to integer"};
    case Type::Struct:
        throw VBRuntimeError{"could not convert struct to integer"};
    }

    return {};
}

DoubleT Value::toFloat() const {
    switch (typeName()) {
    case Type::String:
        return std::stod(get<std::string>());
    case Type::Single:
        return get<SingleT>();
    case Type::Double:
        return get<DoubleT>();
    case Type::Long:
        return get<LongT>();
    case Type::Integer:
        return get<LongT>();
    case Type::Byte:
        return get<ByteT>();
    case Type::Class:
        throw VBRuntimeError{"could not convert class to integer"};
    case Type::Struct:
        throw VBRuntimeError{"could not convert struct to integer"};
    }

    return {};
}

ClassT Value::toClass() const {
    if (value.index() == Type::Class) {
        return get<ClassT>();
    }
    return {};
}

Type Value::commonType(Type other) const {
    if (typeName() == Type::Double || other.type == Type::Double) {
        return {Type::Double};
    }
    if (typeName() == Type::Single || other.type == Type::Single) {
        return {Type::Single};
    }
    if (typeName() == Type::Long || other.type == Type::Long ||
        other.type == Type::Byte) {
        return {Type::Long};
    }
    return {Type::Integer};
}
