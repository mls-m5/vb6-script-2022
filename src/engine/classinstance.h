#pragma once
#include "value.h"
#include <memory>
#include <string_view>
#include <vector>

class ClassType;

class ClassInstance {
    ClassType *_type = nullptr;
    std::vector<Value> _memberValues;

    // Use public function create to create
    ClassInstance(ClassType *type);

    ClassInstance(ClassType *type, std::vector<Value> values);

public:
    size_t getMemberIndex(std::string_view name) const;

    void set(std::string_view name, Value value);

    void set(size_t i, Value value);

    const Value &get(std::string_view name) const;

    const Value &get(size_t i) const;

    Value &get(std::string_view name);

    Value &get(size_t i);

    static std::unique_ptr<ClassInstance> create(ClassType *type);

    std::unique_ptr<ClassInstance> clone();

    ClassType *type();
};
