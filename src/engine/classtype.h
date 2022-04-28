#pragma once
#include "type.h"
#include "value.h"
#include "vbruntimeerror.h"
#include <string>
#include <vector>

struct MemberVariable {
    std::string name;
    Type type;
};

class Function;

class ClassType {
public:
    std::string name;
    std::vector<MemberVariable> variables;
    struct Module *module = nullptr;
    bool isStruct = false;

    ~ClassType();

    void addAddVariable(std::string name, Type variable) {
        variables.push_back(MemberVariable{
            std::move(name),
            std::move(variable),
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
