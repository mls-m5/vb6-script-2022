#pragma once

#include "classinstance.h"
#include "module.h"
#include "vbfwd.h"
#include <filesystem>
#include <iosfwd>

//! Prescan is used to make code find types in other files
//! If not called by the user it is called automatically by loadModule
Module &prescanModule(std::filesystem::path, GlobalContext &global);

Module &loadModule(std::filesystem::path path, GlobalContext &global);

Module &parse(std::istream &stream,
              std::filesystem::path path,
              GlobalContext &global);
