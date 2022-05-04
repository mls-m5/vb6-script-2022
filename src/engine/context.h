#pragma once

#include "functionargument.h"
#include "value.h"
#include <map>
#include <vector>

struct ModuleList : std::vector<std::shared_ptr<class Module>> {};

struct GlobalContext {
    ModuleList modules;
};

struct Module;

struct LocalContext {
    std::vector<Value> localVariables;
    FunctionArgumentValues args;
    Value returnValue; // For functions: return value
    Module *module = nullptr;
    const class FunctionBody *functionBody = nullptr;
    size_t line = 0;
    Value me = Value{Type::Class};
    ValueOrRef with = ValueOrRef{Type::Integer};

    LocalContext(GlobalContext &globalContext,
                 const std::vector<std::pair<std::string, Type>> vars = {},
                 FunctionArgumentValues = {},
                 Module *module = nullptr,
                 Value me = {});

    GlobalContext &globalContext;

    // Source location for error messages
    struct Location currentLocation() const;

    const Function *function(std::string_view name) const;
};
