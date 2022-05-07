#include "classtype.h"
#include "classinstance.h"
#include "function.h"

ClassType::~ClassType() {}

void ClassType::addAddVariable(std::string name,
                               Type type,
                               bool shouldCreateNew) {
    variables.push_back(MemberVariable{
        .name = std::move(name),
        .type = std::move(type),
        .shouldCreateNew = shouldCreateNew,
    });
}

int ClassType::getVariableIndex(std::string_view name) {
    for (size_t i = 0; i < variables.size(); ++i) {
        if (variables.at(i).name == name) {
            return i;
        }
    }
    return -1;
}
