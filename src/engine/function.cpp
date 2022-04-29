#include "function.h"
#include <iostream>

Value Function::call(const FunctionArgumentValues &args,
                     LocalContext &context) const {
    return _f(args, context);
}
