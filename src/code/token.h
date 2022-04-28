#pragma once

#include <string>
#include <vector>

#define TOKEN_KEYWORDS                                                         \
    TOKEN_KEYWORD(Sub)                                                         \
    TOKEN_KEYWORD(Function)                                                    \
    TOKEN_KEYWORD(End)                                                         \
    TOKEN_KEYWORD(If)                                                          \
    TOKEN_KEYWORD(Elif)                                                        \
    TOKEN_KEYWORD(Private)

struct Token {
#define TOKEN_KEYWORD(x) x,
    enum Keyword { NotKeyword, TOKEN_KEYWORDS };
#undef TOKEN_KEYWORD

    std::string content;
    size_t line;

    // Check if token is keyword
    Keyword keyword() const;

    // Do not case sensitive
    bool operator==(std::string_view str);
    bool operator==(const char *c) {
        return *this == std::string_view{c};
    }
};

struct Line : std::vector<Token> {};
