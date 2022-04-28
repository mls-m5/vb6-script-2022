#pragma once
#include "command.h"
#include "context.h"
#include "function.h"
#include <vector>

class FunctionBody {
    using CommandType = std::function<void(LocalContext &)>;

    std::vector<Type> localVariables;

    std::vector<CommandType> commands;

    Value returnValue;

public:
    void pushCommand(const CommandType &t);

    Value call(const FunctionArgumentValues &values,
               LocalContext &context) const;
};
