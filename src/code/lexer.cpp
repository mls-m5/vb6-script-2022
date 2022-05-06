#include "lexer.h"
#include <array>
#include <fstream>

namespace {

using namespace std::literals;

auto multiCharOperators = std::array{
    "<="sv,
    "<<"sv,
    ">>"sv,
    ">="sv,
    "<>"sv,
};

enum CharType {
    None,
    Operator,
    Alpha,
    Num,
    Space,
    Quotation,
};

CharType getType(char c) {
    if (c == '"') {
        return Quotation;
    }
    if (std::isspace(c)) {
        return Space;
    }
    else if (std::isalpha(c) || c == '_') {
        return Alpha;
    }
    else if (std::isdigit(c)) {
        return Num;
    }
    else {
        return Operator;
    }
}

bool isMultiCharOperator(std::string_view op) {
    for (auto multiChar : multiCharOperators) {
        if (multiChar == op) {
            return true;
        }
    }
    return false;
}

Line splitString(std::string str, size_t lineNum) {
    auto line = Line{};

    auto lastType = CharType::None;

    for (auto c : str) {
        if (c == '\'') {
            break;
        }

        auto type = getType(c);

        if (lastType == Quotation) {
            line.back().content.push_back(c);
            if (type == Quotation) {
                lastType = None;
            }
            continue;
        }

        if (type == Space) {
            lastType = Space;
            continue;
        }

        // TODO: Handle numbers starting with '.' like ".34"
        if (type == Num && lastType == Alpha) {
            // Words can contain numbers but not start with numbers
            line.back().content.push_back(c);
            continue;
        }
        else if (c == '.' && lastType == Num) {
            line.back().content.push_back(c);
            continue;
        }
        else if (type != lastType) {
            line.push_back(Token{std::string{c}, lineNum});
        }
        else if (type == Operator) {
            if (isMultiCharOperator(line.back().content + c)) {
                line.back().content.push_back(c);
            }
            else {
                line.push_back(Token{std::string{c}, lineNum});
            }
        }
        else {
            line.back().content.push_back(c);
        }

        lastType = type;
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

    bool shouldMergeLines = false;
    bool haveSkippedHeader = false;
    bool isHeader = false;

    for (std::string line; getline(stream, line); ++l) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (!haveSkippedHeader) {
            if (line.rfind("VERSION ", 0) == 0) {
                for (; getline(stream, line);) {
                    ++l;
                    if (!line.empty() && line.back() == '\r') {
                        line.pop_back();
                    }
                    if (line == "End" || line == "END") {
                        ++l;
                        getline(stream, line);
                        break;
                    }
                }

                haveSkippedHeader = true;
            }
            while ((line.rfind("Attribute ") == 0) && getline(stream, line)) {
                ++l;
                haveSkippedHeader = true;
            };
            haveSkippedHeader = true;
        }

        lines.push_back(splitString(line, l));

        if (lines.back().empty()) {
            lines.pop_back();
            continue;
        }

        if (shouldMergeLines) {
            auto backLine = std::move(lines.back());
            lines.pop_back();

            for (auto &token : backLine) {
                lines.back().push_back(token);
            }
            shouldMergeLines = false;
        }

        if (lines.back().back().content == "_") {
            shouldMergeLines = true;
            lines.back().pop_back();
        }
    }

    auto commonPath = std::make_shared<std::filesystem::path>(path);

    for (auto &line : lines) {
        for (auto &token : line) {
            token.loc.path = commonPath;
        }
    }
}
