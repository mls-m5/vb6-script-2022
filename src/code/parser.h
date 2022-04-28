#pragma once

#include "module.h"
#include <filesystem>
#include <iosfwd>

Module loadModule(std::filesystem::path path);
Module parse(std::istream &stream, std::filesystem::path path);
