#pragma once

#include "Location.h"
#include "keywordlist.h"
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

struct Token {
#define TOKEN_KEYWORD(x) x,
    enum Keyword {
        Word,
        Empty,
        StringLiteral,
        EOFKeyword,
        NumberLiteral,
        Operator,
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
