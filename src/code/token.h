#pragma once

#include <string>
#include <vector>

struct Token {
    std::string content;
    size_t line;

    void finalize();

    // Do not case sensitive
    bool operator==(std::string_view str);
    bool operator==(const char *c) {
        return *this == std::string_view{c};
    }
};

struct Line : std::vector<Token> {};
