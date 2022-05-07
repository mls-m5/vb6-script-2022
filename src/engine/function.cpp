#include "function.h"
#include "classinstance.h"
#include "context.h"
#include "vbruntimeerror.h"
#include <iostream>

Function::Function(std::string name, FunctionArguments args, bool isStatic)
    : _name{std::move(name)}
    , _arguments{std::move(args)}
    , _isStatic{isStatic} {}

Value Function::call(const FunctionArgumentValues &args,
                     Value me,
                     Context &context) const {
    if (args.size() != _arguments.size()) {
        throw VBRuntimeError{context.currentLocation(),
                             "Missmatching number of arguments"};
    }
    return _f(args, me, context);
}

std::string_view Function::name() const {
    return _name;
}

const FunctionArguments &Function::arguments() const {
    return _arguments;
}

int Function::argumentIndex(std::string_view name) const {
    for (size_t i = 0; i < _arguments.size(); ++i) {
        if (_arguments.at(i).name == name) {
            return i;
        }
    }
    return -1;
}

void Function::body(FuncT f) {
    _f = f;
}
