#include "function.h"
#include <iostream>

Value Function::call(const FunctionArgumentValues &args) {
    return f(args);
}
