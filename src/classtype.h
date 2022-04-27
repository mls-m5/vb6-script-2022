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
    std::string name;
    std::vector<MemberVariable> variables;
    std::vector<std::unique_ptr<Function>> memberFunctions;

public:
    ~ClassType();

    void addAddVariable(std::string name, Type variable) {
        variables.push_back(MemberVariable{
            std::move(name),
            std::move(variable),
        });
    }

    friend class ClassInstance;
};
