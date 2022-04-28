#include "functionbody.h"
#include "classinstance.h"

void FunctionBody::pushLocalVariable(Type t) {
    localVariables.push_back(t);
}

void FunctionBody::pushCommand(const CommandT &command) {
    if (!command) {
        return;
    }
    commands.push_back(command);
}

Value FunctionBody::call(const FunctionArgumentValues &args,
                         LocalContext &context) const {
    auto local = LocalContext{
        context.globalContext, localVariables, args, context.module};

    for (auto &command : commands) {
        command(local);
    }

    return local.returnValue;
}
