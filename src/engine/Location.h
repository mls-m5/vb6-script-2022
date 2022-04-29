#pragma once

#include <filesystem>
#include <memory>

struct Location {
    size_t line = 0;
    std::shared_ptr<std::filesystem::path> path;
};
