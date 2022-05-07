#include "simmodule.h"
#include "classtype.h"
#include "context.h"
#include "module.h"

Module &createSimModule(std::filesystem::path path, GlobalContext &global) {
    auto module = std::make_shared<Module>(path);

    if (path.extension() == ".cls" || path.extension() == ".frm") {
        module->classType = std::make_unique<ClassType>();
        module->classType->module = module.get();
        module->classType->name = path.stem();
    }

    global.modules.push_back(module);

    return *module;
}
