#pragma once

#include "classinstance.h"
#include "type.h"
#include "value.h"
#include <string>
#include <vector>

struct FunctionArgument {
    std::string name;
    Type type;
    bool isRef = true;
};

struct FunctionArguments : public std::vector<FunctionArgument> {};

struct FunctionArgumentValue {
    std::variant<Value, Value *> value;
};

struct FunctionArgumentValues : std::vector<FunctionArgumentValue> {};

class Function {
    std::string name;
    FunctionArguments arguments;

public:
    Function(std::string name, FunctionArguments args)
        : name{std::move(name)}
        , arguments{std::move(args)} {}

    Value call(const FunctionArgumentValues &args) {
        return {};
    }
};
