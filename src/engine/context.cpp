#include "context.h"
#include "Location.h"
#include "classinstance.h"
#include "module.h"

Context::Context(GlobalContext &globalContext,
                           const std::vector<std::pair<std::string, Type>> vars,
                           FunctionArgumentValues args,
                           Module *module,
                           Value me)
    : globalContext{globalContext}
    , module{module}
    , me{me} {
    localVariables.reserve(vars.size());
    for (auto &var : vars) {
        localVariables.push_back(Value::create(var.second));
    }

    this->args = std::move(args);
}

Location Context::currentLocation() const {
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

const Function *Context::function(std::string_view name) const {
    for (auto &m : globalContext.modules) {
        if (auto f = m->function(name)) {
            return f;
        }
    }

    return module->function(name);
}
