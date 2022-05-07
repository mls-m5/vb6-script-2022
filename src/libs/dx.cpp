#include "dx.h"
#include "classinstance.h"
#include "classtype.h"
#include "module.h"

namespace {

Module &createModule(std::filesystem::path path, GlobalContext &global) {
    auto module = std::make_shared<Module>(path);

    if (path.extension() == ".cls" || path.extension() == ".frm") {
        module->classType = std::make_unique<ClassType>();
        module->classType->module = module.get();
        module->classType->name = path.stem();
    }

    global.modules.push_back(module);

    return *module;
}

} // namespace

void loadDX7(GlobalContext &global) {
    auto &dx7 = createModule("builtin/DirectX7.cls", global);
    auto &dd7 = createModule("builtin/DirectDraw7.cls", global);
    auto &dds = createModule("builtin/DirectDrawSurface7.cls", global);
    auto &ddc = createModule("builtin/DirectDrawClipper.cls", global);

    auto &dxGlobal = createModule("builtin/DXGlobal.bas", global);

    auto &RECT = dxGlobal.addStruct("RECT");
    RECT.addAddVariable("Left", {Type::Integer}, false);
    RECT.addAddVariable("Top", {Type::Integer}, false);
    RECT.addAddVariable("Right", {Type::Integer}, false);
    RECT.addAddVariable("Bottom", {Type::Integer}, false);
}
