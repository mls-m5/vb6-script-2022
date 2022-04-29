#pragma once

#include "classtype.h"
#include "function.h"
#include <filesystem>

enum class ModuleType {
    Module,
    Class,
};

struct Module {
    std::vector<std::shared_ptr<Function>> functions;
    std::vector<std::pair<std::string, Type>> variables;
    std::vector<std::pair<std::string, Value>> staticVariables;
    std::vector<std::pair<std::string, ClassType>> classes;
    std::filesystem::path path;

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

    ClassType *structType(std::string_view name) {
        for (auto &c : classes) {
            if (c.first == name) {
                return &c.second;
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

    ClassType &addStruct(std::string name) {
        classes.push_back({name, {}});
        classes.back().second.isStruct = true;
        return classes.back().second;
    }
};
