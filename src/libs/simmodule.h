#pragma once

#include "classinstance.h"
#include "engine/vbfwd.h"
#include <filesystem>

Module &createSimModule(std::filesystem::path path, GlobalContext &global);
