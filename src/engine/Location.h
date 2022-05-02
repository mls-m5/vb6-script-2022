#pragma once

#include <filesystem>
#include <memory>

struct Location {
    size_t line = 0;
    std::shared_ptr<std::filesystem::path> path;

    std::string toString() const {
        auto str = std::string{};
        if (path) {
            str += path->string() + ":";
        }

        str += std::to_string(line);

        return str;
    }
};
