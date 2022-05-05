#include "project.h"
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

    for (std::string line; std::getline(file, line);) {
        auto [first, second] = splitLine(line, '=');
        //        std::cout << "property " << first << " with value " << second
        //                  << std::endl;

        if (first == "Class") {
            auto [name, path] = splitLine(second, ';');

            addClass(trim(name), projectPath.parent_path() / trim(path));
        }
    }
}

void Project::addClass(std::string name, std::filesystem::path path) {
    std::cout << "Adding class " << name << " at path " << path << std::endl;

    try {
        loadModule(path, _globalContext.modules);
    }
    catch (VBParsingError &e) {
        std::cerr << e.what() << "\n";
        throw e;
    }
}
