#include "classinstance.h"
#include "classtype.h"
#include "vbruntimeerror.h"

ClassInstance::ClassInstance(ClassType *type)
    : _type{type} {
    _memberValues.reserve(type->variables.size());
    for (auto &t : type->variables) {
        _memberValues.emplace_back(Value::create(t.type));
    }
}

ClassInstance::ClassInstance(ClassType *type, std::vector<Value> values)
    : _type{type}
    , _memberValues{std::move(values)} {}

size_t ClassInstance::getMemberIndex(std::string_view name) const {
    for (size_t i = 0; i < _type->variables.size(); ++i) {
        auto &v = _type->variables.at(i);
        if (v.name == name) {
            return i;
        }
    }

    throw VBRuntimeError{"member " + std::string{name} +
                         " does not exist on type " + _type->name};
}

void ClassInstance::set(std::string_view name, Value value) {
    auto i = getMemberIndex(name);
    auto &v = _memberValues.at(i);
    v = std::move(value);
}

void ClassInstance::set(size_t i, Value value) {
    _memberValues.at(i) = std::move(value);
}

const Value &ClassInstance::get(std::string_view name) const {
    return get(getMemberIndex(name));
}

const Value &ClassInstance::get(size_t i) const {
    return _memberValues.at(i);
}

Value &ClassInstance::get(std::string_view name) {
    return get(getMemberIndex(name));
}

Value &ClassInstance::get(size_t i) {
    return _memberValues.at(i);
}

std::unique_ptr<ClassInstance> ClassInstance::create(ClassType *type) {
    // Using "new" here because the compiler complains about constructor
    // being private otherwise
    return std::unique_ptr<ClassInstance>(new ClassInstance(type));
}

std::unique_ptr<ClassInstance> ClassInstance::clone() {
    return std::unique_ptr<ClassInstance>(
        new ClassInstance(_type, _memberValues));
}

ClassType *ClassInstance::type() {
    return _type;
}
