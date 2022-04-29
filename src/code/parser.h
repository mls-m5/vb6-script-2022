#pragma once

#include "module.h"
#include <filesystem>
#include <iosfwd>

std::unique_ptr<Module> loadModule(std::filesystem::path path);
std::unique_ptr<Module> parse(std::istream &stream, std::filesystem::path path);
