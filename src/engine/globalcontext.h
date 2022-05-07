#pragma once

#include <memory>
#include <random>
#include <vector>

class ClassType;

struct ModuleList : std::vector<std::shared_ptr<class Module>> {};

struct GlobalContext {
    ~GlobalContext();

    ModuleList modules;

    std::unique_ptr<ClassType> nothingType;

    std::mt19937_64 generator;

    GlobalContext();
};
