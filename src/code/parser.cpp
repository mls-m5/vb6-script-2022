#include "parser.h"
#include "engine/function.h"
#include "functionbody.h"
#include "lexer.h"
#include "vbparsingerror.h"
#include <functional>
#include <iostream>

namespace {

enum Scope {
    Public,
    Private,
    None,
};

using NextTokenT = std::function<std::pair<Token *, Token *>()>;
using NextLineT = std::function<Line *()>;
using ExpressionT = std::function<Value(LocalContext &)>;

void parseAssert(bool condition,
                 const Token &location,
                 std::string_view message = "parsing error") {
    if (!condition) {
        throw VBParsingError{location, std::string{message}};
    }
}

FunctionArguments parseArguments(NextTokenT nextToken) {
    auto args = FunctionArguments{};

    // Todo: Parse arguments here

    return args;
}

ExpressionT parseExpression(Token &token, NextTokenT nextToken) {
    auto keyword = token.keyword();

    switch (keyword) {
    case Token::StringLiteral:
        return [str = token.content](LocalContext &) -> Value { return {str}; };
    default:
        throw VBParsingError{token, "unexpected token " + token.content};
    };
}

FunctionBody::CommandT parseCommand(Token *token, NextTokenT nextToken) {
    auto next = nextToken();
    switch (token->keyword()) {
    case Token::Print: {
        parseAssert(next.first, *token, "expected expression");
        auto expr = parseExpression(*next.first, nextToken);

        return [expr](LocalContext &context) {
            std::cout << expr(context).toString() << std::endl; //
        };
    } break;
    default:

        throw VBParsingError{*token, "unexpected command"};
    }

    return {};
}

std::unique_ptr<Function> parseFunction(Line *line,
                                        Scope scope,
                                        NextTokenT nextToken,
                                        NextLineT nextLine) {

    auto token = nextToken();

    auto name = token.first;

    auto arguments = parseArguments(nextToken);

    auto body = std::make_shared<FunctionBody>();

    for (line = nextLine(); line; line = nextLine()) {
        token = nextToken();
        if (!token.first) {
            continue;
        }
        auto keyWord = token.first->keyword();
        if (keyWord == Token::End) {
            break;
        }
        body->pushCommand(parseCommand(token.first, nextToken));
    }

    Function::FuncT f = [body](const FunctionArgumentValues &args,
                               LocalContext &context) -> Value {
        return body->call(args, context);
    };

    auto function =
        std::make_unique<Function>(name->content, std::move(arguments), f);

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

        switch (token.first->keyword()) {
        case Token::Public:
            currentScope = Public;
            break;
        case Token::Private:
            currentScope = Private;
            break;
        case Token::Sub:
            module.addFunction(
                parseFunction(line, Private, nextToken, nextLine));
            continue;
            break;
        default:
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

    size_t lineNum = 0;
    size_t tokenNum = 0;

    Line *line = nullptr;

    auto nextLine = [&lineNum, &f, &tokenNum, &line]() -> Line * {
        tokenNum = 0;
        auto i = lineNum;
        ++lineNum;
        if (i < f.lines.size()) {
            return (line = &f.lines.at(i));
        }
        else {
            return {};
        }
    };

    nextLine();

    auto nextToken = [&tokenNum, &line]() -> std::pair<Token *, Token *> {
        auto i = tokenNum;
        auto i2 = tokenNum + 1;
        ++tokenNum;
        if (!line) {
            return {};
        }
        return {
            (i < line->size()) ? &line->at(i) : nullptr,
            (i2 < line->size()) ? &line->at(i2) : nullptr,
        };
    };

    return parseGlobal(line, nextToken, nextLine);
}