#include "functionbody.h"
#include "classinstance.h"

Type FunctionBody::variable(int i) const {
    return _localVariables.at(i).second;
}

int FunctionBody::variableIndex(std::string_view name) const {
    for (size_t i = 0; i < _localVariables.size(); ++i) {
        if (_localVariables.at(i).first == name) {
            return i;
        }
    }
    return -1;
}

void FunctionBody::pushLocalVariable(std::string name, Type t) {
    for (auto &var : _localVariables) {
        if (var.first == name) {
            throw std::logic_error{"variable already exists"};
        }
    }
    _localVariables.push_back({std::move(name), t});
}

void FunctionBody::forgetLocalVariableName(std::string_view name) {
    for (auto &var : _localVariables) {
        if (var.first == name) {
            var.first = ""; // Simple way to forgetting name
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
                         LocalContext &context) const {
    auto local = LocalContext{
        context.globalContext, _localVariables, args, context.module, me};

    local.functionBody = this;

    for (size_t i = 0; i < _commands.size(); ++i) {
        auto &command = _commands.at(i);
        auto line = _line.at(i);
        local.line = line;
        command(local);
    }

    return local.returnValue;
}
