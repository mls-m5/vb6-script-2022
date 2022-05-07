#pragma once

#include "functionbody.h"
#include <functional>

class CodeBlock {
    using CommandT = FunctionBody::CommandT;
    std::vector<FunctionBody::CommandT> _commands;

public:
    // TODO: Include location in CommandT and
    void addCommand(const CommandT &command);

    ReturnT run(Context &context) const;
};
