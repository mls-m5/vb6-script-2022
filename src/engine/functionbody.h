#pragma once
#include "Location.h"
#include "context.h"
#include "function.h"
#include "variabledescription.h"
#include <vector>

enum class ReturnT {
    Standard = 0,
    Return,
    ExitFor,
    ContinueFor,
    ExitFunction, // Same as exit sub
};

struct CommandT {
    using FuncT = std::function<ReturnT(Context &)>;
    FuncT f;

    template <typename F>
    CommandT(F f)
        : f{f} {}

    CommandT &operator=(FuncT f) {
        this->f = f;
        return *this;
    }

    CommandT() = default;

    ReturnT operator()(Context &context) const {
        return f(context);
    }

    operator bool() const {
        return static_cast<bool>(f);
    }
};

class FunctionBody {
public:
    using CommandT = ::CommandT;

private:
    std::vector<ArgumentDescription> _localVariables;
    std::vector<CommandT> _commands;
    std::vector<size_t> _line;

    Value _returnValue;
    Function *_function;

public:
    Type variable(int i) const;

    int variableIndex(std::string_view name) const;

    void pushLocalVariable(std::string, Type t, bool shouldCreateNew);
    void forgetLocalVariableName(std::string_view);

    void pushCommand(const CommandT &t, size_t line);

    Value call(const FunctionArgumentValues &values,
               Value me,
               Context &context) const;

    void function(Function *function) {
        _function = function;
    }

    const Function *function() const {
        return _function;
    }
};
