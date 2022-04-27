#pragma once

#include "type.h"
#include <string>
#include <vector>

struct FunctionArgument {
    std::string name;
    Type type;
    bool isRef = true;
};

class Function {
    std::string name;
    std::vector<FunctionArgument> arguments;
};
