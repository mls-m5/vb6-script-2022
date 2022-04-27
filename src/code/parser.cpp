#include "parser.h"
#include "engine/function.h"
#include "lexer.h"
#include "vbparsingerror.h"
#include <functional>

namespace {

enum Scope {
    Public,
    Private,
    None,
};

using NextTokenT = std::function<std::pair<Token *, Token *>()>;
using NextLineT = std::function<Line *()>;

FunctionArguments parseArguments(NextTokenT nextToken) {
    auto args = FunctionArguments{};

    // Todo: Parse arguments here

    return args;
}

std::unique_ptr<Function> parseFunction(Line *line,
                                        Scope scope,
                                        NextTokenT nextToken,
                                        NextLineT nextLine) {

    auto token = nextToken();

    auto name = token.first;

    auto f = [](const FunctionArgumentValues &, LocalContext &context) {
        return Value{};
    };

    auto function =
        std::make_unique<Function>(name->content, parseArguments(nextToken), f);

    return function;
}

Module parseGlobal(Line *line, NextTokenT nextToken, NextLineT nextLine) {
    auto module = Module{};

    Scope currentScope;

    for (; line; line = nextLine()) {
        currentScope = Private;
        auto token = nextToken();
        if (!token.first) {
            continue;
        }

        if (*token.first == "Public") {
            currentScope = Public;
        }
        else if (*token.first == "Private") {
            currentScope = Private;
        }
        else if (*token.first == "Sub") {
            module.addFunction(
                parseFunction(line, Private, nextToken, nextLine));
        }
        else {
            throw VBParsingError{*token.first,
                                 "unexpected token at global scope"};
        }

        token = nextToken();

        if (!token.first) {
            continue;
        }

        if (*token.first == "Sub") {
            module.addFunction(
                parseFunction(line, currentScope, nextToken, nextLine));
        }
    }

    return module;
}

} // namespace

Module parse(std::istream &stream, std::filesystem::path path) {

    auto f = CodeFile{stream, path};

    size_t tokenNum = 0;

    auto nextLine = [i = size_t{0}, &f, &tokenNum]() mutable -> Line * {
        tokenNum = 0;
        if (i < f.lines.size()) {
            return &f.lines.at(i++);
        }
        else {
            ++i;
            return {};
        }
    };

    auto line = nextLine();

    auto nextToken = [&tokenNum, &line]() -> std::pair<Token *, Token *> {
        auto i2 = tokenNum + 1;
        if (!line) {
            return {};
        }
        return {
            (tokenNum < line->size()) ? &line->at(tokenNum) : nullptr,
            (i2 < line->size()) ? &line->at(i2) : nullptr,
        };
    };

    return parseGlobal(line, nextToken, nextLine);
}
