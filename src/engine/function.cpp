#include "function.h"
#include <iostream>

Value Function::call(const FunctionArgumentValues &args,
                     Value me,
                     Context &context) const {
    return _f(args, me, context);
}
