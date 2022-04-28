#include "token.h"
#include <algorithm>
#include <unordered_map>

namespace {

std::string toLower(std::string str) {
    for (auto &c : str) {
        c = std::tolower(c);
    }

    return str;
}

bool iCompare(std::string_view lowerString, std::string ordinary) {
    return lowerString == toLower(ordinary);
}

Token::Keyword getKeyword(std::string name) {
    name = toLower(name);

#define TOKEN_KEYWORD(x) {toLower(#x), Token::x},

    static const auto map =
        std::unordered_map<std::string, Token::Keyword>{TOKEN_KEYWORDS};

    if (auto f = map.find(name); f != map.end()) {
        return f->second;
    }

    return Token::NotKeyword;
}
} // namespace

bool Token::operator==(std::string_view str) {
    if (str.size() != content.size()) {
        return false;
    }
    for (size_t i = 0; i < str.size(); ++i) {
        if (std::tolower(str.at(i)) != std::tolower(content.at(i))) {
            return false;
        }
    }

    return true;
}

Token::Keyword Token::keyword() const {
    return getKeyword(content);
}
