#include "parser.h"
#include "engine/function.h"
#include "functionbody.h"
#include "lexer.h"
#include "vbparsingerror.h"
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>

namespace {

enum Scope {
    Public,
    Private,
    None,
};

using NextTokenT = std::function<std::pair<Token *, Token *>()>;

// Contains the current state of the parser
struct TokenPair {
    TokenPair() = default;
    TokenPair(const TokenPair &) = delete;
    TokenPair &operator=(const TokenPair &) = delete;

    NextTokenT f;

    Location lastLoc; // Last valid code location
    Token *first = nullptr;
    Token *second = nullptr;

    Module *currentModule = nullptr;

    FunctionBody *currentFunctionBody = nullptr;
    std::vector<std::string> namedLocalVariables;
    std::vector<std::string> namedArguments;

    void endFunction() {
        currentFunctionBody = nullptr;
        namedLocalVariables.clear();
        namedArguments.clear();
    }

    int localVariable(std::string_view name) {
        for (size_t i = 0; i < namedLocalVariables.size(); ++i) {
            if (namedLocalVariables.at(i) == name) {
                return i;
            }
        }

        return -1;
    }

    // Get index to argument in current context
    int argument(std::string_view name) {
        for (size_t i = 0; i < namedArguments.size(); ++i) {
            if (namedArguments.at(i) == name) {
                return i;
            }
        }

        return -1;
    }

    TokenPair &next() {
        std::tie(first, second) = f();
        if (first) {
            lastLoc = first->loc;
        }
        return *this;
    }

    operator bool() {
        return first;
    }

    std::string content() const {
        return first ? first->content : "";
    }

    Token::Keyword type() const {
        return first ? first->type() : Token::Empty;
    }
};

using NextLineT = std::function<Line *()>;
using ExpressionT = std::function<Value(LocalContext &)>;

void parseAssert(bool condition,
                 const Location &location,
                 std::string_view message = "parsing error") {
    if (!condition) {
        throw VBParsingError{location, std::string{message}};
    }
}

Type parseAsStatement(TokenPair &token) {
    auto loc = token.first->loc;
    token.next();

    if (!token.first) {
        throw VBParsingError{loc, "expected typename"};
    }

    auto type = token.first->type();

    token.next();
    //    std::cout << type << std::endl;
    // TODO: Implement types
    return Type{Type::Integer};
}

FunctionArguments parseArguments(TokenPair &token) {
    if (!token || token.first->content != "(") {
        throw VBParsingError{token.lastLoc,
                             "expected character '(' got " + token.content()};
    }

    auto args = FunctionArguments{};

    // TODO: Handle ByRef and ByVal

    token.next();

    for (bool shouldContinue = true; shouldContinue;) {
        if (token.content() == ")") {
            return args;
        }

        auto name = token.content();
        token.next();

        auto type = (token.type() == Token::As) ? parseAsStatement(token)
                                                : Type{Type::Integer};

        args.push_back({type, name});
        token.namedArguments.push_back(name);

        if (token.content() == ",") {
            shouldContinue = true;
            token.next();
        }
    }

    // TODO: Parse the whole list separated by ','

    return args;
}

std::function<Value &(LocalContext &context)> parseIdentifier(
    TokenPair &token) {
    auto loc = token.first->loc;
    auto name = token.first->content;

    token.next();

    if (auto index = token.localVariable(name); index != -1) {
        return [index](LocalContext &context) -> Value & {
            return context.localVariables.at(index);
        };
    }

    if (auto index = token.argument(name); index != -1) {
        return [index](LocalContext &context) -> Value & {
            return context.args.at(index).get();
        };
    }

    if (auto index = token.currentModule->staticVariable(name); index != -1) {
        return [index](LocalContext &context) -> Value & {
            return context.module->staticVariables.at(index).second;
        };
    }

    throw VBParsingError{loc, "could not find variable " + name};
}

ExpressionT parseExpression(TokenPair &token) {
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

    case Token::Word:
        expr = parseIdentifier(token);
        break;
    default:
        throw VBParsingError{*token.first,
                             "unexpected token " + token.first->content};
    };

    if (!expr) {
        throw VBParsingError{*token.first, "failied to parse expression"};
    }

    token.next();

    if (!token.first) {
        return expr;
    }

    // TODO: Handle for exemple binary expressions
    switch (token.first->type()) {
    default:
        return expr;
    }
}

std::vector<ExpressionT> parseList(TokenPair &token) {
    auto list = std::vector<ExpressionT>{};

    if (!token.first) {
        return {};
    }

    list.push_back(parseExpression(token));

    while (token.content() == ",") {
        auto loc = token.first->loc;
        token.next();
        if (!token.first) {
            throw VBParsingError{loc, "expected expression"};
        }
        list.push_back(parseExpression(token));
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

FunctionBody::CommandT parseMethodCall(TokenPair &token) {
    auto methodName = token.first->content;

    token.next();
    auto argExpressionList = parseList(token);

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

void parseLocalVariableDeclaration(TokenPair &token) {
    if (!token.currentFunctionBody) {
        throw VBInternalParsingError{*token.first, "no function body"};
    }

    auto name = token.first->content;

    token.namedLocalVariables.push_back(name);

    token.next();

    if (token && token.type() == Token::As) {
        auto type = parseAsStatement(token);
        token.currentFunctionBody->pushLocalVariable(type);
    }
    else {
        token.currentFunctionBody->pushLocalVariable({Type::Integer});
    }
}

void parseMemberDeclaration(TokenPair &token) {
    auto loc = token.first->loc;
    if (token.type() != Token::Word) {
        token.next();
        parseAssert(token, loc, "Expected variable name");
    }

    auto name = token.content();

    token.next();

    // TODO: Handle differences between class and modules where modules always
    // have global/static variables and classes as standard have nonstatic
    // variables
    if (false) {
        if (token && token.type() == Token::As) {
            token.currentModule->variables.push_back(
                {name, parseAsStatement(token)});
        }
    }
    else {
        if (token && token.type() == Token::As) {
            token.currentModule->staticVariables.push_back(
                {name, Value::create(parseAsStatement(token))});
        }
    }
}

void parseMemberDeclarationList(TokenPair &token) {
    parseMemberDeclaration(token);

    // TODO: Loop for all declarations
}

FunctionBody::CommandT parseAssignment(TokenPair &token) {
    auto loc = token.first->loc;
    auto target = parseIdentifier(token);
    token.next();

    if (!token) {
        throw VBParsingError{loc, "Expected expression to assign to"};
    }

    auto expr = parseExpression(token);

    return [target, expr](LocalContext &context) {
        target(context) = expr(context);
    };
}

//! A command is a line inside a function that starts at the beginning of the
//! line
FunctionBody::CommandT parseCommand(TokenPair &token) {
    switch (token.first->type()) {
    case Token::Print: {
        parseAssert(token.second, *token.first, "expected expression");
        token.next();
        auto expr = parseExpression(token);

        return [expr](LocalContext &context) {
            std::cout << expr(context).toString() << std::endl; //
        };
    } break;

    case Token::Dim:
        token.next();
        parseLocalVariableDeclaration(token);

        break;

    case Token::Word: {
        auto type2 = token.second ? token.second->type() : Token::Empty;
        // TODO: Handle other cases, like assignments and stuff
        if (type2 == Token::Operator) {
            return parseAssignment(token);
        }
        else {
            return parseMethodCall(token);
        }
    }
    default:

        throw VBParsingError{*token.first,
                             "unexpected command " + token.first->content};
    }

    return {};
}

std::unique_ptr<Function> parseFunction(Line *line,
                                        Scope scope,
                                        TokenPair &token,
                                        NextLineT nextLine) {

    token.next();

    auto name = token.first;

    token.next();

    auto arguments = parseArguments(token);

    auto body = std::make_shared<FunctionBody>();

    token.currentFunctionBody = body.get();

    for (line = nextLine(); line; line = nextLine()) {
        token.next();
        if (!token.first) {
            continue;
        }
        auto keyWord = token.first->type();
        if (keyWord == Token::End) {
            break;
        }
        body->pushCommand(parseCommand(token));
    }

    Function::FuncT f = [body](const FunctionArgumentValues &args,
                               LocalContext &context) -> Value {
        return body->call(args, context);
    };

    auto function =
        std::make_unique<Function>(name->content, std::move(arguments), f);

    token.endFunction();

    return function;
}

Module parseGlobal(Line *line, NextTokenT nextToken, NextLineT nextLine) {
    auto module = Module{};

    Scope currentScope;

    for (; line; line = nextLine()) {
        currentScope = Private;
        TokenPair token{};
        token.f = nextToken;
        token.currentModule = &module;
        token.next();

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
            module.addFunction(parseFunction(line, Private, token, nextLine));
            continue;
            break;
        case Token::Option:
            continue; // Skip option statements, assume Option Explicit
        case Token::Dim:
            parseMemberDeclaration(token);
            continue;
        default:
            throw VBParsingError{*token.first,
                                 "unexpected token at global scope: " +
                                     token.content()};
        }

        token.next();

        if (token) {
            continue;
        }

        switch (token.first->type()) {
        case Token::Sub:
            module.addFunction(
                parseFunction(line, currentScope, token, nextLine));
            break;

        case Token::Word:
            parseMemberDeclaration(token);
            break;

        default:
            throw VBParsingError{*token.first,
                                 "unexpected token at global scope " +
                                     token.first->content};
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

Module loadModule(std::filesystem::path path) {
    auto file = std::ifstream{path};
    return parse(file, path);
}
