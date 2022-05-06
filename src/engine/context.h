#pragma once

#include "functionargument.h"
#include "value.h"
#include "variabledescription.h"
#include <map>
#include <random>
#include <vector>

struct ModuleList : std::vector<std::shared_ptr<class Module>> {};

struct GlobalContext {
    ModuleList modules;

    std::unique_ptr<ClassType> nothingType;

    std::mt19937_64 generator;

    GlobalContext();
};

struct Module;

struct Context {
    std::vector<Value> localVariables;
    FunctionArgumentValues args;
    Value returnValue; // For functions: return value
    Module *module = nullptr;
    const class FunctionBody *functionBody = nullptr;
    size_t line = 0;
    Value me = Value{Type::Class};
    ValueOrRef with = ValueOrRef{Type::Integer};

    Context(GlobalContext &globalContext,
            const std::vector<ArgumentDescription> vars = {},
            FunctionArgumentValues = {},
            Module *module = nullptr,
            Value me = {});

    GlobalContext &globalContext;

    // Source location for error messages
    struct Location currentLocation() const;

    const Function *function(std::string_view name) const;
};
