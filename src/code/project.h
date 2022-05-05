#pragma once

#include "module.h"
#include <filesystem>

struct Project {
    Project(std::string name);

private:
    void parseProjectFile(std::filesystem::path path);

    void addClass(std::string name, std::filesystem::path path);

    GlobalContext _globalContext;
    Context _context;
};
