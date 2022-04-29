#include "context.h"
#include "Location.h"
#include "classinstance.h"
#include "module.h"

LocalContext::LocalContext(GlobalContext &globalContext,
                           const std::vector<Type> vars,
                           FunctionArgumentValues args,
                           Module *module)
    : globalContext{globalContext}
    , module{module} {
    localVariables.reserve(vars.size());
    for (auto &var : vars) {
        localVariables.push_back(Value::create(var));
    }

    this->args = std::move(args);
}

Location LocalContext::currentLocation() const {
    if (module) {
        return Location{
            line,
            std::make_shared<std::filesystem::path>(module->path),
        };
    }

    auto loc = Location{
        line,
        std::make_shared<std::filesystem::path>("<no file>"),
    };

    return loc;
}
