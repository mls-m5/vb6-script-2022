#include "codeblock.h"
#include "classinstance.h"

void CodeBlock::addCommand(const CommandT &command) {
    if (command) {
        _commands.push_back(command);
    }
    else {
        throw std::runtime_error{
            "trying to add empty block"}; // For testingi only
    }
}

ReturnT CodeBlock::run(Context &context) const {
    for (auto &command : _commands) {
        if (auto t = command(context); t != ReturnT::Standard) {
            return t;
        }
    }
    return ReturnT::Standard;
}
