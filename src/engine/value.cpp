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

Value::Value(ArrayT array)
    : value(std::move(array)) {}

Value &Value::operator=(Value &&other) {
    if (other.value.index() == value.index()) {
        value = std::move(other.value);
        return *this;
    }

    switch (typeName()) {
    case Type::String:
        value = other.toString();
        break;
    case Type::Single:
        value = static_cast<SingleT>(other.toFloat());
        break;
    case Type::Double:
        value = static_cast<DoubleT>(other.toFloat());
        break;
    case Type::Long:
        value = static_cast<LongT>(other.toInteger());
        break;
    case Type::Integer:
        value = static_cast<IntegerT>(other.toInteger());
        break;
    case Type::Byte:
        value = static_cast<ByteT>(other.toInteger());
        break;
    case Type::Class:
        value = other.get<ClassT>();
        break;
    case Type::Struct:
        value = other.get<StructT>();
        break;
    case Type::Array:
        value = other.get<ArrayT>();
        break;
    }

    return *this;
}

Value &Value::operator=(const Value &other) {
    if (other.value.index() == value.index()) {
        value = other.value;
        return *this;
    }

    switch (typeName()) {
    case Type::String:
        value = other.toString();
        break;
    case Type::Single:
        value = static_cast<SingleT>(other.toFloat());
        break;
    case Type::Double:
        value = static_cast<DoubleT>(other.toFloat());
        break;
    case Type::Long:
        value = static_cast<LongT>(other.toInteger());
        break;
    case Type::Integer:
        value = static_cast<IntegerT>(other.toInteger());
        break;
    case Type::Byte:
        value = static_cast<ByteT>(other.toInteger());
        break;
    case Type::Class:
        value = other.get<ClassT>();
        break;
    case Type::Struct:
        value = other.get<StructT>();
        break;
    case Type::Array:
        value = other.get<ArrayT>();
        break;
    }

    return *this;
}

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
    case Type::Class:
        return Value{ClassT{}};
    case Type::Struct:
        return Value{StructT{type.classType}};
    case Type::Array:
        throw VBRuntimeError{
            "cannot create array value without underlying type"};
    }

    throw VBRuntimeError{"invalid type"};
}

void Value::forceSet(Value value) {
    this->value = std::move(value.value);
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
        auto c = get<ClassT>().get();
        if (!c) {
            throw VBRuntimeError{"trying to access membor of null value"};
        }
        classType = c->type();
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
    case Type::Array:
        throw VBRuntimeError{"could not convert array to string"};
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
    case Type::Array:
        throw VBRuntimeError{"could not array class to integer"};
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
    case Type::Array:
        throw VBRuntimeError{"could not convert array to float"};
    case Type::Class:
        throw VBRuntimeError{"could not convert class to float"};
    case Type::Struct:
        throw VBRuntimeError{"could not convert struct to float"};
    }

    return {};
}

bool Value::isNumber() const {
    switch (typeName()) {
    case Type::String:
    case Type::Array:
    case Type::Class:
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
    case Type::Byte:
        return -get<ByteT>();
    case Type::Array:
        throw VBRuntimeError{"could not negate array value"};
    case Type::Class:
        throw VBRuntimeError{"could not negate class value"};
    case Type::Struct:
        throw VBRuntimeError{"could not negate struct value"};
    }

    return {};
}
