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
    bool isArray = false;
    int lowerBound = 0;
    int upperBound = 0;
};
