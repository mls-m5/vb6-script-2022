#include "context.h"
#include "classinstance.h"
#include "classtype.h"
#include "globalcontext.h"
#include "location.h"
#include "module.h"

Context::Context(GlobalContext &globalContext,
                 const std::vector<ArgumentDescription> vars,
                 FunctionArgumentValues args,
                 Module *module,
                 Value me)
    : globalContext{globalContext}
    , module{module}
    , me{me} {
    localVariables.reserve(vars.size());
    for (auto &var : vars) {
        if (var.shouldCreateNew) {
            localVariables.push_back(
                Value{ClassInstance::create(var.type.classType)});
        }
        else {
            localVariables.push_back(Value::create(var.type));
        }
    }

    this->args = std::move(args);
}

Location Context::currentLocation() const {
    if (module) {
        return Location{
            line,
            std::make_shared<std::filesystem::path>(module->path()),
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
