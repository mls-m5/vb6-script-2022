#include "functionbody.h"
#include "classinstance.h"

void FunctionBody::pushCommand(const CommandType &command) {
    commands.push_back(command);
}

Value FunctionBody::call(const FunctionArgumentValues &args,
                         LocalContext &context) const {
    auto local = LocalContext{context.globalContext, localVariables, args};

    for (auto &command : commands) {
        command(local);
    }

    return local.returnValue;
}
