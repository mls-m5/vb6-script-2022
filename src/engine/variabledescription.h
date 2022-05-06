#pragma once

#include "type.h"
#include <string>

struct ArgumentDescription {
    std::string name;
    Type type;
    bool shouldCreateNew = false;
};

struct TypeDescription {
    Type type;
    bool shouldCreateNew = false;
};
