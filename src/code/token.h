#pragma once

#include "location.h"
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
        FloatLiteral,
        Operator,
        Period,
        ParenthesesBegin,
        ParenthesesEnd,
        TOKEN_KEYWORD_LIST
    };
#undef TOKEN_KEYWORD

    std::string content;
    Location loc;

    // Check if token is keyword
    Keyword type() const;

    operator Location() const;

    // Do not case sensitive
    bool operator==(std::string_view str);
    bool operator==(const char *c);
};

struct Line : std::vector<Token> {};

bool isOperator(Token::Keyword);
