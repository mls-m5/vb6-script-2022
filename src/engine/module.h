#pragma once

#include "value.h"
#include "vbfwd.h"
#include <filesystem>
#include <vector>

enum class ModuleType {
    Module,
    Class,
};

struct Module {
    struct StaticVariable {
        std::string name;
        Value value;
        Type type;
        bool shouldCreateNew = false;
    };

private:
    std::vector<StaticVariable> staticVariables;

    std::vector<std::shared_ptr<Function>> _functions;
    std::vector<std::shared_ptr<ClassType>> _classes;
    std::filesystem::path _path;

public:
    Module() = default;
    Module(const Module &) = delete;
    Module &operator=(const Module &) = delete;
    ~Module() = default;

    Module(std::filesystem::path path);

    std::unique_ptr<ClassType> classType;

    //! Init static variables
    void init();

    ModuleType type();

    Function &addFunction(std::shared_ptr<Function> function);

    std::string name() const;

    std::filesystem::path path() const;

    Function *function(std::string_view name) const;

    ClassType *structType(std::string_view name);

    int staticVariableIndex(std::string_view name);

    Value &staticVariable(int index);

    void addStaticVariable(std::string name,
                           Type type,
                           bool shouldCreateNew = false);

    ClassType &addStruct(std::string name);
};
