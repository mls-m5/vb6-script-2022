#pragma once
#include "Location.h"
#include "context.h"
#include "function.h"
#include <vector>

class FunctionBody {
public:
    using CommandT = std::function<void(LocalContext &)>;

private:
    std::vector<Type> _localVariables;
    std::vector<CommandT> _commands;
    std::vector<size_t> _line;

    Value _returnValue;
    Function *_function;

public:
    //    Type variable(int i);
    Type variable(int i) const;

    void pushLocalVariable(Type t);

    void pushCommand(const CommandT &t, size_t line);

    Value call(const FunctionArgumentValues &values,
               LocalContext &context) const;

    void function(Function *function) {
        _function = function;
    }

    const Function *function() const {
        return _function;
    }
};
