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
    using FuncT = std::function<Value(
        const FunctionArgumentValues &, Value me, LocalContext &context)>;

private:
    std::string _name;
    FunctionArguments _arguments;
    FuncT _f;
    bool _isStatic = true;

public:
    Function(std::string name, FunctionArguments args, FuncT f, bool isStatic)
        : _name{std::move(name)}
        , _arguments{std::move(args)}
        , _f{f}
        , _isStatic{isStatic} {}

    Value call(const FunctionArgumentValues &args,
               Value me,
               LocalContext &context) const;

    std::string_view name() const {
        return _name;
    }

    const FunctionArguments &arguments() const {
        return _arguments;
    }

    int argumentIndex(std::string_view name) const {
        for (size_t i = 0; i < _arguments.size(); ++i) {
            if (_arguments.at(i).name == name) {
                return i;
            }
        }
        return -1;
    }
};
