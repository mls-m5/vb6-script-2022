#pragma once

#include "context.h"
#include "module.h"

// Set of stuff to send into function calls
struct TestContext {
    GlobalContext global;
    Context local{global};
    FunctionArgumentValues args;

    TestContext(Module *module)
        : local{global, {}, {}, module} {}
};
