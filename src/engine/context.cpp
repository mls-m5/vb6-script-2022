#include "context.h"
#include "classinstance.h"

LocalContext::LocalContext(GlobalContext &globalContext,
                           const std::vector<Type> vars,
                           FunctionArgumentValues args,
                           const Module *module)
    : globalContext{globalContext}
    , module{module} {
    localVariables.reserve(vars.size());
    for (auto &var : vars) {
        localVariables.push_back(Value::create(var));
    }

    this->args = std::move(args);
}
