#include "functionbody.h"
#include "classinstance.h"

// Type FunctionBody::variable(int i) {
//     return _localVariables.at(i);
// }

Type FunctionBody::variable(int i) const {
    return _localVariables.at(i);
}

void FunctionBody::pushLocalVariable(Type t) {
    _localVariables.push_back(t);
}

void FunctionBody::pushCommand(const CommandT &command, size_t line) {
    if (!command) {
        return;
    }
    _commands.push_back(command);
    _line.push_back(line);
}

Value FunctionBody::call(const FunctionArgumentValues &args,
                         LocalContext &context) const {
    auto local = LocalContext{
        context.globalContext, _localVariables, args, context.module};

    local.functionBody = this;

    //    for (auto &command : _commands) {
    for (size_t i = 0; i < _commands.size(); ++i) {
        auto &command = _commands.at(i);
        auto line = _line.at(i);
        local.line = line;
        command(local);
    }

    return local.returnValue;
}
