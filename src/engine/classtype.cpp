#include "classtype.h"
#include "function.h"
#include "module.h"

ClassType::~ClassType() {}

Function *ClassType::function(std::string_view name) {
    return module->function(name);
}

Function *ClassType::function(int i) {
    return module->functions.at(i).get();
}
