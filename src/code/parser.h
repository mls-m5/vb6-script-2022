#pragma once

#include "module.h"
#include <filesystem>
#include <iosfwd>

Module parse(std::istream &stream, std::filesystem::path path);
