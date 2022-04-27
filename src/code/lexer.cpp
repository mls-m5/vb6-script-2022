#include "lexer.h"
#include <fstream>

namespace {

enum CharType {
    None,
    Operator,
    Alpha,
    Num,
    Space,
};

CharType getType(char c) {
    if (std::isspace(c)) {
        return Space;
    }
    else if (std::isalpha(c)) {
        return Alpha;
    }
    else if (std::isdigit(c)) {
        return Num;
    }
    else {
        return Operator;
    }
}

Line splitString(std::string str, size_t lineNum) {
    auto line = Line{};

    auto lastType = CharType::None;

    for (auto c : str) {
        auto type = getType(c);

        if (type == Space) {
            lastType = Space;
            continue;
        }

        if (type != lastType || type == Operator) {
            line.push_back(Token{std::string{c}, lineNum});
        }
        else {
            line.back().content.push_back(c);
        }

        lastType = type;
        ++lineNum;
    }

    return line;
}
} // namespace

CodeFile::CodeFile(std::istream &stream, std::filesystem::path path) {
    load(stream, path);
}

CodeFile::CodeFile(std::filesystem::path path) {
    auto file = std::ifstream{path};
    load(file, path);
}

void CodeFile::load(std::istream &stream, std::filesystem::path path) {
    int l = 1;
    for (std::string line; getline(stream, line); ++l) {
        lines.push_back(splitString(line, l));
    }
}
