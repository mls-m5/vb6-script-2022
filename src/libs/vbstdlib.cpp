#include "vbstdlib.h"
#include "simmodule.h"

void loadVbStdLib(GlobalContext &global) {
    auto &module = createSimModule("builtin/Form.cls", global);
}
