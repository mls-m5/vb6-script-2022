#pragma once

#include "context.h"

// Set of stuff to send into function calls
struct TestContext {
    GlobalContext global;
    LocalContext local{global};
    FunctionArgumentValues args;
};
