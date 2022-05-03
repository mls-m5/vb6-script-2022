#pragma once

#include "functionbody.h"
#include <functional>

class CodeBlock {
    using CommandT = FunctionBody::CommandT;
    std::vector<FunctionBody::CommandT> _commands;

public:
    // TODO: Include location in CommandT and
    void addCommand(const CommandT &command) {
        if (command) {
            _commands.push_back(command);
        }
        else {
            throw std::runtime_error{
                "trying to add empty block"}; // For testingi only
        }
    }

    ReturnT run(LocalContext &context) const {
        for (auto &command : _commands) {
            if (auto t = command(context); t != ReturnT::Standard) {
                return t;
            }
        }
        return ReturnT::Standard;
    }

    //    auto begin() {
    //        return _commands.begin();
    //    }

    //    auto begin() const {
    //        return _commands.begin();
    //    }

    //    auto end() {
    //        return _commands.end();
    //    }

    //    auto end() const {
    //        return _commands.end();
    //    }
};
