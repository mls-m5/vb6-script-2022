#pragma once
#include "type.h"
#include "value.h"
#include "vbruntimeerror.h"
#include <string>
#include <vector>

struct MemberVariable {
    std::string name;
    Type type;
    bool shouldCreateNew = false;
};

class Function;

class ClassType {
public:
    std::string name;
    std::vector<MemberVariable> variables;
    struct Module *module = nullptr;
    bool isStruct = false;

    ClassType() = default;
    ClassType(const ClassType &) = delete;
    ClassType(ClassType &&) = default;
    ClassType &operator=(const ClassType &) = delete;
    ClassType &operator=(ClassType &&) = default;

    ~ClassType();

    void addAddVariable(std::string name, Type type, bool shouldCreateNew) {
        variables.push_back(MemberVariable{
            .name = std::move(name),
            .type = std::move(type),
            .shouldCreateNew = shouldCreateNew,
        });
    }

    int getVariableIndex(std::string_view name) {
        for (size_t i = 0; i < variables.size(); ++i) {
            if (variables.at(i).name == name) {
                return i;
            }
        }
        return -1;
    }

    friend class ClassInstance;
};
