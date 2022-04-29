#pragma once

#include "classinstance.h"
#include "context.h"
#include "type.h"
#include "value.h"
#include <functional>
#include <string>
#include <vector>

struct FunctionArgument {
    Type type;
    std::string name;
    bool isByRef = true;
};

struct FunctionArguments : public std::vector<FunctionArgument> {};

class Function {
public:
    using FuncT = std::function<Value(const FunctionArgumentValues &,
                                      LocalContext &context)>;

private:
    std::string _name;
    FunctionArguments _arguments;
    FuncT _f;

public:
    Function(std::string name, FunctionArguments args, FuncT f)
        : _name{std::move(name)}
        , _arguments{std::move(args)}
        , _f{f} {}

    Value call(const FunctionArgumentValues &args, LocalContext &context) const;

    std::string_view name() const {
        return _name;
    }

    const FunctionArguments &arguments() const {
        return _arguments;
    }
};
