#pragma once

#include "functionargument.h"
#include "value.h"
#include <map>
#include <vector>

struct GlobalContext {
    std::map<std::string, Value> globalVariables;

    void set(const std::string &name, Value value) {
        globalVariables[name] = std::move(value);
    }

    Value &get(const std::string &name) {
        return globalVariables.at(name);
    }

    const Value &get(const std::string &name) const {
        return globalVariables.at(name);
    }
};

struct LocalContext {
    std::vector<Value> localVariables;
    FunctionArgumentValues args;
    Value returnValue; // For functions: return value

    LocalContext(GlobalContext &globalContext,
                 const std::vector<Type> vars = {},
                 FunctionArgumentValues = {});

    GlobalContext &globalContext;
};
