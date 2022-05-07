#pragma once

#include "functionargument.h"
#include "type.h"
#include <functional>
#include <string>

class Context;

struct FunctionArgument {
    Type type;
    std::string name;
    bool isByRef = true;
    bool shouldCreateNew = false;
    // If 'default value' is set, the argument is Optional
    std::function<ValueOrRef(Context &localcontext)> defaultValue = {};
};

struct FunctionArguments : public std::vector<FunctionArgument> {};

class Function {
public:
    using FuncT = std::function<Value(
        const FunctionArgumentValues &, Value me, Context &context)>;

private:
    std::string _name;
    FunctionArguments _arguments;
    FuncT _f;
    bool _isStatic = true;

public:
    Function(std::string name, FunctionArguments args, bool isStatic);

    Value call(const FunctionArgumentValues &args,
               Value me,
               Context &context) const;

    std::string_view name() const;

    const FunctionArguments &arguments() const;

    int argumentIndex(std::string_view name) const;

    void body(FuncT f);
};
