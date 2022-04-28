#pragma once

#include <string>
#include <vector>

#include "keywordlist.h"

struct Token {
#define TOKEN_KEYWORD(x) x,
    enum Keyword {
        NotKeyword,
        Empty,
        StringLiteral,
        EOFKeyword,
        TOKEN_KEYWORD_LIST
    };
#undef TOKEN_KEYWORD

    std::string content;
    size_t line;

    // Check if token is keyword
    Keyword type() const;

    // Do not case sensitive
    bool operator==(std::string_view str);
    bool operator==(const char *c) {
        return *this == std::string_view{c};
    }
};

struct Line : std::vector<Token> {};
