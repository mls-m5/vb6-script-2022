#pragma once
#include "type.h"
#include <string>
#include <vector>

struct MemberVariable {
    std::string name;
    Type type;
    bool shouldCreateNew = false;
};

class Function;

class ClassType {
public:
    std::string name;
    std::vector<MemberVariable> variables;
    struct Module *module = nullptr;
    bool isStruct = false;

    ClassType() = default;
    ClassType(const ClassType &) = delete;
    ClassType(ClassType &&) = default;
    ClassType &operator=(const ClassType &) = delete;
    ClassType &operator=(ClassType &&) = default;

    ~ClassType();

    //! @param shouldCreateNew is only relevant for class variables
    void addAddVariable(std::string name, Type type, bool shouldCreateNew);

    int getVariableIndex(std::string_view name);

    friend class ClassInstance;
};
