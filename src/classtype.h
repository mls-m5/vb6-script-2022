#pragma once
#include "function.h"
#include "type.h"
#include "value.h"
#include "vbruntimeerror.h"
#include <string>
#include <vector>

struct MemberVariable {
    std::string name;
    Type type;
};

class ClassType {
    std::string name;
    std::vector<MemberVariable> variables;

public:
    void addAddVariable(std::string name, Type variable) {
        variables.push_back(MemberVariable{
            std::move(name),
            std::move(variable),
        });
    }

    friend class ClassInstance;
};
