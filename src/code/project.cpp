#include "project.h"
#include "classinstance.h"
#include "parser.h"
#include "vbparsingerror.h"
#include <fstream>
#include <iostream>

namespace {

std::string trim(std::string str) {
    while (!str.empty() && std::isspace(str.front())) {
        str.erase(0, 1);
    }

    while (!str.empty() && std::isspace(str.back())) {
        str.pop_back();
    }

    return str;
}

std::pair<std::string, std::string> splitLine(std::string line, char c) {
    if (auto f = line.find(c); f != std::string::npos) {
        return {line.substr(0, f), line.substr(f + 1, line.size() - 1)};
    }
    else {
        return {line, ""};
    }
}

} // namespace

Project::Project(std::string name)
    : _context{_globalContext} {
    parseProjectFile(name);
}

void Project::parseProjectFile(std::filesystem::path projectPath) {
    auto file = std::ifstream{projectPath};

    if (!file) {
        throw std::runtime_error{"could not open file " + projectPath.string()};
    }

    auto moduleList = std::vector<std::filesystem::path>{};

    for (std::string line; std::getline(file, line);) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        auto [first, second] = splitLine(line, '=');

        if (first == "Class" || first == "Module") {
            auto [name, path] = splitLine(second, ';');

            moduleList.push_back(trim(path));
        }
    }

    for (auto &path : moduleList) {
        prescanModule(path, _globalContext);
    }

    for (auto &path : moduleList) {
        addModule(path.stem(), projectPath.parent_path() / trim(path));
    }

    for (auto &mod : _globalContext.modules) {
        mod->init();
    }
}

void Project::addModule(std::string name, std::filesystem::__cxx11::path path) {
    std::cout << "Adding module " << name << " at path " << path << std::endl;

    try {
        loadModule(path, _globalContext);
    }
    catch (VBParsingError &e) {
        std::cerr << e.what() << "\n";
        throw e;
    }
}
