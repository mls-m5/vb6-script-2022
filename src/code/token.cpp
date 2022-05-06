#include "token.h"
#include <algorithm>
#include <array>
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
    constexpr static auto operators = std::string_view{"=*+-<>/\\%&@!#$^"};

    if (name.empty()) {
        return Token::Empty;
    }

    if (name.front() == '"') {
        return Token::StringLiteral;
    }

    if (operators.find(name.front()) != std::string::npos) {
        return Token::Operator;
    }

    if (name == ".") {
        return Token::Period;
    }
    else if (name == "(") {
        return Token::ParenthesesBegin;
    }
    else if (name == ")") {
        return Token::ParenthesesEnd;
    }

    if (std::isdigit(name.front())) {
        if (name.find('.') != std::string::npos) {
            return Token::FloatLiteral;
        }
        else {
            return Token::NumberLiteral;
        }
    }

    name = toLower(name);

#define TOKEN_KEYWORD(x) {toLower(#x), Token::x},

    static const auto map = std::unordered_map<std::string, Token::Keyword>{
        {"EOF", Token::EOFKeyword}, TOKEN_KEYWORD_LIST};

    if (auto f = map.find(name); f != map.end()) {
        return f->second;
    }

    return Token::Word;
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

Token::Keyword Token::type() const {
    return getKeyword(content);
}

bool isOperator(Token::Keyword keyword) {
    switch (keyword) {
    case Token::Operator:
    case Token::Not:
    case Token::And:
    case Token::Xor:
    case Token::Or:
    case Token::TypeOf:
    case Token::Is:
        return true;
    default:
        return false;
    }
}
