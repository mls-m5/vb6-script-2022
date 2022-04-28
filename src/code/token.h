#pragma once

#include <string>
#include <vector>

#include "keywordlist.h"

struct Location {
    size_t line = 0;
};

struct Token {
#define TOKEN_KEYWORD(x) x,
    enum Keyword {
        NotKeyword,
        Empty,
        StringLiteral,
        EOFKeyword,
        NumberLiteral,
        TOKEN_KEYWORD_LIST
    };
#undef TOKEN_KEYWORD

    std::string content;
    Location loc;

    // Check if token is keyword
    Keyword type() const;

    operator Location() const {
        return loc;
    }

    // Do not case sensitive
    bool operator==(std::string_view str);
    bool operator==(const char *c) {
        return *this == std::string_view{c};
    }
};

struct Line : std::vector<Token> {};
