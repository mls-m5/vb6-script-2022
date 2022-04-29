#pragma once

#include "functionargument.h"
#include "value.h"
#include <map>
#include <vector>

struct ModuleList : std::vector<std::shared_ptr<class Module>> {};

struct GlobalContext {
    ModuleList modules;

    //    std::map<std::string, Value> globalVariables;

    //    void set(const std::string &name, Value value) {
    //        globalVariables[name] = std::move(value);
    //    }

    //    Value &get(const std::string &name) {
    //        return globalVariables.at(name);
    //    }

    //    const Value &get(const std::string &name) const {
    //        return globalVariables.at(name);
    //    }
};

struct Module;

struct LocalContext {
    std::vector<Value> localVariables;
    FunctionArgumentValues args;
    Value returnValue; // For functions: return value
    Module *module = nullptr;
    const class FunctionBody *functionBody = nullptr;
    size_t line = 0;

    LocalContext(GlobalContext &globalContext,
                 const std::vector<Type> vars = {},
                 FunctionArgumentValues = {},
                 Module *module = {});

    GlobalContext &globalContext;

    // Source location for error messages
    struct Location currentLocation() const;
};
