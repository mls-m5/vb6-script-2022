#pragma once
#include "classtype.h"

class ClassInstance {
    ClassType *type = nullptr;
    std::vector<Value> memberValues;
    std::vector<Function> memberFunctions;

    ClassInstance(ClassType *type)
        : type{type} {
        memberValues.resize(type->variables.size());
    }

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

    void set(std::string_view name, const Value &value) {
        auto i = getMemberIndex(name);
        auto &v = memberValues.at(i);
        v = value;
    }

    const Value &get(std::string_view name) const {
        return memberValues.at(getMemberIndex(name));
    }
};
