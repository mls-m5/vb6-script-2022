#pragma once

#include "function.h"

struct Module {
    std::vector<std::shared_ptr<Function>> functions;
    std::vector<std::pair<std::string, Type>> variables;

    void addFunction(std::shared_ptr<Function> function) {
        functions.push_back(std::move(function));
    }

    Function *function(std::string_view name) const {
        for (auto &f : functions) {
            if (f->name() == name) {
                return f.get();
            }
        }

        return nullptr;
    }
};
