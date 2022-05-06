#pragma once

#include "classtype.h"
#include "function.h"
#include <filesystem>

enum class ModuleType {
    Module,
    Class,
};

struct Module {
    struct StaticVariable {
        std::string name;
        Value value;
        Type type;
        bool shouldCreateNew = false;
    };

private:
    std::vector<StaticVariable> staticVariables;

public:
    Module() = default;
    Module(const Module &) = delete;
    Module &operator=(const Module &) = delete;
    ~Module() = default;

    std::vector<std::shared_ptr<Function>> functions;
    std::vector<std::shared_ptr<ClassType>> classes;
    std::filesystem::path path;

    std::unique_ptr<ClassType> classType;

    //! Init static variables
    void init() {
        for (auto &var : staticVariables) {
            if (var.shouldCreateNew) {
                if (var.type.type != Type::Class || !var.type.classType) {
                    throw VBRuntimeError{"variable " + var.name +
                                         " is not of class type"};
                }
                auto type = var.type.classType;
                var.value = Value{ClassInstance::create(type)};
            }
        }
    }

    ModuleType type() {
        return classType ? ModuleType::Class : ModuleType::Module;
    }

    Function &addFunction(std::shared_ptr<Function> function) {
        functions.push_back(std::move(function));
        return *functions.back();
    }

    std::string name() {
        return path.stem();
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
            // TODO: Make case insensitive comparison struct
            if (c->name == name) {
                return c.get();
            }
        }

        return nullptr;
    }

    int staticVariableIndex(std::string_view name) {
        for (size_t i = 0; i < staticVariables.size(); ++i) {
            // TODO: Make case insensitive comparison struct variables
            if (staticVariables.at(i).name == name) {
                return i;
            }
        }

        return -1;
    }

    Value &staticVariable(int index) {
        return staticVariables.at(index).value;
    }

    void addStaticVariable(std::string name,
                           Type type,
                           bool shouldCreateNew = false) {
        staticVariables.push_back({
            name,
            Value::create(type),
            type,
            shouldCreateNew,
        });
    }

    ClassType &addStruct(std::string name) {
        classes.push_back(std::make_shared<ClassType>());
        auto &s = *classes.back();
        s.name = name;
        s.isStruct = true;
        return s;
    }
};
