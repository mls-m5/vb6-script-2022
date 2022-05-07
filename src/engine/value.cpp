#include "value.h"
#include "classinstance.h"
#include "vbruntimeerror.h"

bool Value::operator==(const Value &other) const {
    return toInteger() == other.toInteger();
}

Value::Value(IntegerT i)
    : value{i} {}

Value::Value(std::string str)
    : value{std::move(str)} {}

Value::Value(SingleT d)
    : value{d} {}

Value::Value(DoubleT d)
    : value{d} {}

Value::Value(LongT l)
    : value{l} {}

Value::Value(ClassT i)
    : value{std::move(i)} {}

Value::Value(StructT i)
    : value{std::move(i)} {}

void Value::forceSet(Value &other) {
    value = std::move(other.value);
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
        //    case Type::Boolean:
        //        return Value{BoolT{0}};
    case Type::Class:
        return Value{ClassT{}};
    case Type::Struct:
        return Value{StructT{type.classType}};
    }

    throw VBRuntimeError{"invalid type"};
}

Type::TypeName Value::typeName() const {
    return static_cast<Type::TypeName>(value.index());
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
        //    case Type::Boolean:
        //        return std::to_string(get<BoolT>());
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

bool Value::isNumber() const {
    switch (typeName()) {
    case Type::String:
        return false;
    case Type::Class:
        return false;
    case Type::Struct:
        return false;
    default:
        return true;
    }

    return false;
}

bool Value::toBool() const {
    return toInteger();
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

Value Value::negative() const {
    switch (typeName()) {
    case Type::String:
        return {-toFloat()};
    case Type::Single:
        return -get<SingleT>();
    case Type::Double:
        return -get<DoubleT>();
    case Type::Long:
        return -get<LongT>();
    case Type::Integer:
        return -get<IntegerT>();
        //    case Type::Boolean:
        //        return get<BoolT>();
    case Type::Byte:
        return -get<ByteT>();
    case Type::Class:
        throw VBRuntimeError{"could not negate class type"};
    case Type::Struct:
        throw VBRuntimeError{"could not negate struct type"};
    }

    return {};
}
