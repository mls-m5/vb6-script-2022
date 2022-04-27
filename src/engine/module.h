#pragma once

#include "function.h"

struct Module {
    std::vector<std::shared_ptr<Function>> functions;
    std::vector<std::pair<std::string, Type>> variables;

    void addFunction(std::shared_ptr<Function> function) {
        functions.push_back(std::move(function));
    }
};
