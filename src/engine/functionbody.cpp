#include "functionbody.h"
#include "classinstance.h"

Type FunctionBody::variable(int i) const {
    return _localVariables.at(i).type;
}

int FunctionBody::variableIndex(std::string_view name) const {
    for (size_t i = 0; i < _localVariables.size(); ++i) {
        if (_localVariables.at(i).name == name) {
            return i;
        }
    }
    return -1;
}

void FunctionBody::pushLocalVariable(std::string name,
                                     Type t,
                                     bool shouldCreateNew) {
    for (auto &var : _localVariables) {
        if (var.name == name) {
            throw std::logic_error{"variable already exists"};
        }
    }
    _localVariables.push_back({std::move(name), t, shouldCreateNew});
}

void FunctionBody::forgetLocalVariableName(std::string_view name) {
    for (auto &var : _localVariables) {
        if (var.name == name) {
            var.name = ""; // Simple way to forgetting name
            return;
        }
    }
}

void FunctionBody::pushCommand(const CommandT &command, size_t line) {
    if (!command) {
        return;
    }
    _commands.push_back(command);
    _line.push_back(line);
}

Value FunctionBody::call(const FunctionArgumentValues &args,
                         Value me,
                         Context &context) const {
    auto local = Context{
        context.globalContext, _localVariables, args, context.module, me};

    local.functionBody = this;
    local.returnValue.forceSet(Value::create(_function->returnType()));

    for (size_t i = 0; i < _commands.size(); ++i) {
        auto &command = _commands.at(i);
        auto line = _line.at(i);
        local.line = line;
        try {
            command(local);
        }
        catch (std::runtime_error &e) {
            throw VBRuntimeError{"in " + local.currentLocation().toString() +
                                 ":\n" + e.what()};
        }
    }

    return local.returnValue;
}

void FunctionBody::function(Function *function) {
    _function = function;
}

const Function *FunctionBody::function() const {
    return _function;
}
