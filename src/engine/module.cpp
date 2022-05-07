#include "module.h"
#include "classinstance.h"
#include "classtype.h"
#include "function.h"

Module::Module(std::filesystem::path path)
    : _path{path} {}

void Module::init() {
    for (auto &var : staticVariables) {
        if (var.shouldCreateNew) {
            if (var.type.type != Type::Class || !var.type.classType) {
                throw VBRuntimeError{"variable " + var.name +
                                     " is not of class type"};
            }
            auto type = var.type.classType;
            var.value = Value{ClassInstance::create(type)};
        }
    }
}

ModuleType Module::type() {
    return classType ? ModuleType::Class : ModuleType::Module;
}

Function &Module::addFunction(std::shared_ptr<Function> function) {
    _functions.push_back(std::move(function));
    return *_functions.back();
}

std::string Module::name() const {
    return _path.stem();
}

std::filesystem::path Module::path() const {
    return _path;
}

Function *Module::function(std::string_view name) const {
    for (auto &f : _functions) {
        if (f->name() == name) {
            return f.get();
        }
    }

    return nullptr;
}

ClassType *Module::structType(std::string_view name) {
    for (auto &c : _classes) {
        // TODO: Make case insensitive comparison struct
        if (c->name == name) {
            return c.get();
        }
    }

    return nullptr;
}

int Module::staticVariableIndex(std::string_view name) {
    for (size_t i = 0; i < staticVariables.size(); ++i) {
        // TODO: Make case insensitive comparison struct variables
        if (staticVariables.at(i).name == name) {
            return i;
        }
    }

    return -1;
}

Value &Module::staticVariable(int index) {
    return staticVariables.at(index).value;
}

void Module::addStaticVariable(std::string name,
                               Type type,
                               bool shouldCreateNew) {
    staticVariables.push_back({
        name,
        Value::create(type),
        type,
        shouldCreateNew,
    });
}

ClassType &Module::addStruct(std::string name) {
    _classes.push_back(std::make_shared<ClassType>());
    auto &s = *_classes.back();
    s.name = name;
    s.isStruct = true;
    return s;
}
