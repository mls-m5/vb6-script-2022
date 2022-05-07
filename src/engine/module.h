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

    std::vector<std::shared_ptr<Function>> _functions;
    std::vector<std::shared_ptr<ClassType>> _classes;
    std::filesystem::path _path;

public:
    Module() = default;
    Module(const Module &) = delete;
    Module &operator=(const Module &) = delete;
    ~Module() = default;

    Module(std::filesystem::path path)
        : _path{path} {}

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
        _functions.push_back(std::move(function));
        return *_functions.back();
    }

    std::string name() const {
        return _path.stem();
    }

    std::filesystem::path path() const {
        return _path;
    }

    Function *function(std::string_view name) const {
        for (auto &f : _functions) {
            if (f->name() == name) {
                return f.get();
            }
        }

        return nullptr;
    }

    ClassType *structType(std::string_view name) {
        for (auto &c : _classes) {
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
        _classes.push_back(std::make_shared<ClassType>());
        auto &s = *_classes.back();
        s.name = name;
        s.isStruct = true;
        return s;
    }
};
