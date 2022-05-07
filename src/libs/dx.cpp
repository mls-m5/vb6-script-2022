#include "dx.h"
#include "classinstance.h"
#include "classtype.h"
#include "module.h"
#include "simmodule.h"

void loadDX7(GlobalContext &global) {
    auto &dx7 = createSimModule("builtin/DirectX7.cls", global);
    auto &dd7 = createSimModule("builtin/DirectDraw7.cls", global);
    auto &dds = createSimModule("builtin/DirectDrawSurface7.cls", global);
    auto &ddc = createSimModule("builtin/DirectDrawClipper.cls", global);

    auto &dxGlobal = createSimModule("builtin/DXGlobal.bas", global);

    auto &RECT = dxGlobal.addStruct("RECT");
    RECT.addAddVariable("Left", {Type::Integer}, false);
    RECT.addAddVariable("Top", {Type::Integer}, false);
    RECT.addAddVariable("Right", {Type::Integer}, false);
    RECT.addAddVariable("Bottom", {Type::Integer}, false);
}
