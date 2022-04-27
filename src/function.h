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
    bool isRef = true;
};

struct FunctionArguments : public std::vector<FunctionArgument> {};

class Function {
public:
    using FuncT = std::function<Value(const FunctionArgumentValues &,
                                      LocalContext &context)>;

private:
    std::string name;
    FunctionArguments arguments;
    FuncT f;

public:
    Function(std::string name, FunctionArguments args, FuncT f)
        : name{std::move(name)}
        , arguments{std::move(args)}
        , f{f} {}

    Value call(const FunctionArgumentValues &args, LocalContext &context);
};
