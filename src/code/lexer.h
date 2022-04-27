#pragma once

#include <filesystem>
#include <string>
#include <vector>

struct Token {
    std::string content;
    size_t line;
};

struct Line : std::vector<Token> {};

struct CodeFile {
    std::filesystem::path path;
    std::vector<Line> lines;

    //! Load stream but call it something else
    CodeFile(std::istream &stream, std::filesystem::path path);

    void load(std::istream &stream, std::filesystem::path path);

    CodeFile(std::filesystem::path);
};
