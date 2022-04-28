#pragma once

#include "function.h"

enum class ModuleType {
    Module,
    Class,
};

struct Module {
    std::vector<std::shared_ptr<Function>> functions;
    std::vector<std::pair<std::string, Type>> variables;
    std::vector<std::pair<std::string, Value>> staticVariables;
    ModuleType type = ModuleType::Module;

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

    int staticVariable(std::string_view name) {
        for (size_t i = 0; i < staticVariables.size(); ++i) {
            if (staticVariables.at(i).first == name) {
                return i;
            }
        }

        return -1;
    }
};
