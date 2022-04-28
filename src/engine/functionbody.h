#pragma once
#include "context.h"
#include "function.h"
#include <vector>

class FunctionBody {
public:
    using CommandT = std::function<void(LocalContext &)>;

private:
    std::vector<Type> localVariables;
    std::vector<CommandT> commands;

    Value returnValue;

public:
    Type variable(int i);

    void pushLocalVariable(Type t);

    void pushCommand(const CommandT &t);

    Value call(const FunctionArgumentValues &values,
               LocalContext &context) const;
};
