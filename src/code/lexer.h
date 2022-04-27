#pragma once

#include "token.h"
#include <filesystem>

struct CodeFile {
    std::filesystem::path path;
    std::vector<Line> lines;

    //! Load stream but call it something else
    CodeFile(std::istream &stream, std::filesystem::path path);

    void load(std::istream &stream, std::filesystem::path path);

    CodeFile(std::filesystem::path);
};
