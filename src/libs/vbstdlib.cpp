#include "vbstdlib.h"
#include "function.h"
#include "module.h"
#include "simmodule.h"

void loadVbStdLib(GlobalContext &global) {
    createSimModule("builtin/Form.cls", global);

    auto &stdMod = createSimModule("builtin/std.bas", global);

    // TODO: Add arguments later
    auto function = std::make_unique<Function>(
        "MsgBox", FunctionArguments{}, Type{Type::Integer}, true);

    stdMod.addFunction(std::move(function));
}
