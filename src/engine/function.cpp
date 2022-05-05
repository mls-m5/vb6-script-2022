#include "function.h"
#include <iostream>

Value Function::call(const FunctionArgumentValues &args,
                     Value me,
                     Context &context) const {
    if (args.size() != _arguments.size()) {
        throw VBRuntimeError{context.currentLocation(),
                             "Missmatching number of arguments"};
    }
    return _f(args, me, context);
}
