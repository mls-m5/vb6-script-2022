#pragma once

#include "value.h"
#include <memory>
#include <random>
#include <vector>

class ClassType;

struct ModuleList : std::vector<std::shared_ptr<class Module>> {};

class GlobalContext {
private:
    Value _err = Value{Type::Integer};

    bool _disableEnd = false;

public:
    // TODO: Refactor this: Encapsulate
    ModuleList modules;
    std::unique_ptr<ClassType> nothingType;
    std::mt19937_64 generator;

    Value err() const;
    void err(Value value);

    void end();
    // For testing.. If std::exit should be called by a End statement
    void disableEnd(bool value);

    ~GlobalContext();
    GlobalContext();
};
