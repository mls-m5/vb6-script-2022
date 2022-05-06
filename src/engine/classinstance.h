#pragma once
#include "classtype.h"

class ClassInstance {
    ClassType *_type = nullptr;
    std::vector<Value> _memberValues;

    // Use public function create to create
    ClassInstance(ClassType *type)
        : _type{type} {
        _memberValues.reserve(type->variables.size());
        for (auto &t : type->variables) {
            _memberValues.emplace_back(Value::create(t.type));
        }
    }

    ClassInstance(ClassType *type, std::vector<Value> values)
        : _type{type}
        , _memberValues{std::move(values)} {}

public:
    size_t getMemberIndex(std::string_view name) const {
        for (size_t i = 0; i < _type->variables.size(); ++i) {
            auto &v = _type->variables.at(i);
            if (v.name == name) {
                return i;
            }
        }

        throw VBRuntimeError{"member " + std::string{name} +
                             " does not exist on type " + _type->name};
    }

    void set(std::string_view name, Value value) {
        auto i = getMemberIndex(name);
        auto &v = _memberValues.at(i);
        v = std::move(value);
    }

    void set(size_t i, Value value) {
        _memberValues.at(i) = std::move(value);
    }

    const Value &get(std::string_view name) const {
        return get(getMemberIndex(name));
    }

    const Value &get(size_t i) const {
        return _memberValues.at(i);
    }

    Value &get(std::string_view name) {
        return get(getMemberIndex(name));
    }

    Value &get(size_t i) {
        return _memberValues.at(i);
    }

    static std::unique_ptr<ClassInstance> create(ClassType *type) {
        // Using "new" here because the compiler complains about constructor
        // being private otherwise
        return std::unique_ptr<ClassInstance>(new ClassInstance(type));
    }

    std::unique_ptr<ClassInstance> clone() {
        return std::unique_ptr<ClassInstance>(
            new ClassInstance(_type, _memberValues));
    }

    ClassType *type() {
        return _type;
    }
};
