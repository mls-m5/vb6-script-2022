#include "dx.h"
#include "classinstance.h"
#include "classtype.h"
#include "module.h"

namespace {

std::shared_ptr<Module> createModuleAndClass(std::filesystem::path path) {
    auto module = std::make_shared<Module>(path);

    if (path.extension() == ".cls" || path.extension() == ".frm") {
        module->classType = std::make_unique<ClassType>();
        module->classType->module = module.get();
        module->classType->name = path.stem();
    }

    return module;
}

} // namespace

void loadDX7(GlobalContext &global) {
    auto dx7 = createModuleAndClass("builtin/DirectX7.cls");
    auto dd7 = createModuleAndClass("builtin/DirectDraw7.cls");
    auto dds = createModuleAndClass("builtin/DirectDrawSurface7.cls");
    auto ddc = createModuleAndClass("builtin/DirectDrawClipper.cls");

    global.modules.push_back(dx7);
    global.modules.push_back(dd7);
    global.modules.push_back(dds);
    global.modules.push_back(ddc);
}
