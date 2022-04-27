#pragma once
#include "classtype.h"

class ClassInstance {
    ClassType *type = nullptr;
    std::vector<Value> memberValues;

    // Use public function create to create
    ClassInstance(ClassType *type)
        : type{type} {
        memberValues.resize(type->variables.size());
    }

public:
    size_t getMemberIndex(std::string_view name) const {
        for (size_t i = 0; i < type->variables.size(); ++i) {
            auto &v = type->variables.at(i);
            if (v.name == name) {
                return i;
            }
        }

        throw VBRuntimeError{"member " + std::string{name} +
                             " does not exist on type " + type->name};
    }

    void set(std::string_view name, Value value) {
        auto i = getMemberIndex(name);
        auto &v = memberValues.at(i);
        v = std::move(value);
    }

    const Value &get(std::string_view name) const {
        return memberValues.at(getMemberIndex(name));
    }

    static std::unique_ptr<ClassInstance> create(ClassType *type) {
        // Using "new" here because the compiler complains about constructor
        // being private otherwise
        return std::unique_ptr<ClassInstance>(new ClassInstance(type));
    }
};
