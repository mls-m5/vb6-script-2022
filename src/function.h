#pragma once

#include "classinstance.h"
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

struct FunctionArgumentValue {
    std::variant<Value, Value *> value;

    Value &get() {
        if (value.index()) {
            return *std::get<Value *>(value);
        }
        else {
            return std::get<Value>(value);
        }
    }

    const Value &get() const {
        if (value.index()) {
            return *std::get<Value *>(value);
        }
        else {
            return std::get<Value>(value);
        }
    }
};

// First argument is "this"-argument
using FunctionArgumentValues = std::vector<FunctionArgumentValue>;

class Function {
public:
    using FuncT = std::function<Value(const FunctionArgumentValues &)>;

private:
    std::string name;
    FunctionArguments arguments;
    FuncT f;

public:
    Function(std::string name, FunctionArguments args, FuncT f)
        : name{std::move(name)}
        , arguments{std::move(args)}
        , f{f} {}

    Value call(const FunctionArgumentValues &args);
};
