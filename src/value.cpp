#include "value.h"
#include "classinstance.h"

Value Value::create(Type type) {
    switch (type.type) {
    case Type::String:
        return Value{std::string{}};
    case Type::Single:
        return Value{1.f};
    case Type::Double:
        return Value{1.};
    case Type::Long:
        return Value{LongT{0}};
    case Type::Integer:
        return Value{IntegerT{0}};
    case Type::Class:
        return Value{std::shared_ptr<ClassInstance>{}};
    case Type::Struct:
        return Value{std::unique_ptr<ClassInstance>{}};
    }

    return {};
}
