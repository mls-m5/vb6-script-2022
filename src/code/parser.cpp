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

using TokenPair = std::pair<Token *, Token *>;
using NextTokenT = std::function<TokenPair()>;
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

    // TODO: Parse arguments here

    return args;
}

ExpressionT parseExpression(TokenPair &token, NextTokenT nextToken) {
    auto keyword = token.first->type();

    ExpressionT expr;

    switch (keyword) {
    case Token::StringLiteral:
        expr = [str = token.first->content](LocalContext &) {
            return Value{str};
        };
        break;
    case Token::NumberLiteral:
        // Todo: Handle floating point
        expr = [i = std::stoi(token.first->content)](LocalContext &) {
            return Value{IntegerT{i}};
        };
        break;

    default:
        throw VBParsingError{*token.first,
                             "unexpected token " + token.first->content};
    };

    token = nextToken();

    if (!token.first) {
        return expr;
    }

    // TODO: Handle for exemple binary expressions
    switch (token.first->type()) {
    default:
        return expr;
    }
}

std::vector<ExpressionT> parseList(TokenPair &token, NextTokenT nextToken) {
    auto list = std::vector<ExpressionT>{};

    list.push_back(parseExpression(token, nextToken));

    while (token.first && token.first->content == ",") {
        auto loc = token.first->loc;
        token = nextToken();
        if (!token.first) {
            throw VBParsingError{loc, "expected expression"};
        }
        list.push_back(parseExpression(token, nextToken));
    }

    return list;
}

Function *lookupFunction(std::string_view name, LocalContext &context) {
    if (context.module) {
        if (auto f = context.module->function(name)) {
            return f;
        }

        throw VBRuntimeError{"function " + std::string{name} +
                             " does not exist"};
    }

    throw VBRuntimeError{
        "trying to lookup function on context without active module"};
};

FunctionArgumentValues evaluateArgumentList(
    const std::vector<ExpressionT> &args, LocalContext &context) {
    auto values = FunctionArgumentValues{};
    values.reserve(args.size());

    for (auto &arg : args) {
        // TODO: Handle references
        values.push_back(FunctionArgumentValue{arg(context)});
    }

    return values;
}

FunctionBody::CommandT parseMethodCall(TokenPair token, NextTokenT nextToken) {
    auto methodName = token.first->content;

    token = nextToken();
    auto argExpressionList = parseList(token, nextToken);

    return [methodName,
            argExpressionList,
            function = static_cast<Function *>(nullptr)](
               LocalContext &context) mutable {
        if (!function) {
            function = lookupFunction(methodName, context);
        }

        if (!function) {
            VBRuntimeError{"could not find function " + methodName};
        }

        function->call(evaluateArgumentList(argExpressionList, context),
                       context);
    };
}

//! A command is a line inside a function that starts at the beginning of the
//! line
FunctionBody::CommandT parseCommand(TokenPair token, NextTokenT nextToken) {
    switch (token.first->type()) {
    case Token::Print: {
        parseAssert(token.second, *token.first, "expected expression");
        auto next = nextToken();
        auto expr = parseExpression(next, nextToken);

        return [expr](LocalContext &context) {
            std::cout << expr(context).toString() << std::endl; //
        };
    } break;

    case Token::NotKeyword:
        // TODO: Handle other cases, like assignments and stuff
        return parseMethodCall(token, nextToken);
    default:

        throw VBParsingError{*token.first, "unexpected command"};
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
        auto keyWord = token.first->type();
        if (keyWord == Token::End) {
            break;
        }
        body->pushCommand(parseCommand(token, nextToken));
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

        switch (token.first->type()) {
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
        case Token::Option:
            continue; // Skip option statements, assume Option Explicit
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
