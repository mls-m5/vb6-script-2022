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

class ClassType {
    std::string name;
    std::vector<MemberVariable> variables;

    friend class ClassInstance;
};
