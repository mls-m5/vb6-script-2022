#include "function.h"
#include <iostream>

Value Function::call(const FunctionArgumentValues &args,
                     Value me,
                     LocalContext &context) const {
    return _f(args, me, context);
}
