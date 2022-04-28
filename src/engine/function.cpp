#include "function.h"
#include <iostream>

Value Function::call(const FunctionArgumentValues &args, LocalContext &context) {
    return _f(args, context);
}
