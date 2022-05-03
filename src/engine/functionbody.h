#pragma once
#include "Location.h"
#include "context.h"
#include "function.h"
#include <vector>

enum class ReturnT {
    Standard = 0,
    Return,
    ExitFor,
    Continue,
};

struct CommandT {
    using FuncT = std::function<ReturnT(LocalContext &)>;
    FuncT f;

    template <typename F>
    CommandT(F f)
        : f{f} {}

    CommandT &operator=(FuncT f) {
        this->f = f;
        return *this;
    }

    CommandT() = default;

    ReturnT operator()(LocalContext &context) const {
        return f(context);
    }

    operator bool() const {
        return static_cast<bool>(f);
    }
};

class FunctionBody {
public:
    //    using CommandT = std::function<void(LocalContext &)>;
    using CommandT = ::CommandT;

private:
    std::vector<std::pair<std::string, Type>> _localVariables;
    std::vector<CommandT> _commands;
    std::vector<size_t> _line;

    Value _returnValue;
    Function *_function;

public:
    Type variable(int i) const;

    int variableIndex(std::string_view name) const;

    void pushLocalVariable(std::string, Type t);
    void forgetLocalVariableName(std::string_view);

    void pushCommand(const CommandT &t, size_t line);

    Value call(const FunctionArgumentValues &values,
               Value me,
               LocalContext &context) const;

    void function(Function *function) {
        _function = function;
    }

    const Function *function() const {
        return _function;
    }
};
