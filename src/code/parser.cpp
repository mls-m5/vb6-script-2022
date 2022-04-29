#include "parser.h"
#include "engine/function.h"
#include "functionbody.h"
#include "lexer.h"
#include "parsetypes.h"
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
    std::vector<std::shared_ptr<Module>> modules;

    FunctionBody *currentFunctionBody = nullptr;
    std::vector<std::string> namedLocalVariables;
    std::vector<std::pair<std::string, Type>> namedArguments;

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
            if (namedArguments.at(i).first == name) {
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
        return first ? first->content : "End of file";
    }

    Token::Keyword type() const {
        return first ? first->type() : Token::Empty;
    }

    ClassType *structType(std::string_view name) const {
        if (currentModule) {
            if (auto t = currentModule->structType(name)) {
                return t;
            }
        }

        for (auto &m : modules) {
            if (auto t = m->structType(name)) {
                return t;
            }
        }

        return nullptr;
    }

    ClassType *classType(std::string_view name) const {
        for (auto &m : modules) {
            if (m->type() == ModuleType::Class && m->name() == name) {
                return m->classType.get();
            }
        }

        return nullptr;
    }

    Function *function(std::string_view name) const {
        for (auto &m : modules) {
            if (m->type() == ModuleType::Module) {
                if (auto f = m->function(name)) {
                    return f;
                }
            }
        }
        return nullptr;
    }
};

struct MethodReferenceReturn {
    Function *f = nullptr;

    operator bool() const {
        return f;
    }
};

using NextLineT = std::function<Line *()>;
using ExpressionT = std::function<ValueOrRef(LocalContext &)>;
using MethodReferenceT = std::function<MethodReferenceReturn(LocalContext &)>;

using IdentifierFuncT = std::variant<ExpressionT, MethodReferenceT>;
using IdentifierFuncResult = std::variant<ValueOrRef, MethodReferenceReturn>;

struct LateIdentifierResult {
    ValueOrRef vor; // Only filled if result is value type
    IdentifierFuncT f;
};

using LateIdentifierFunc = std::function<LateIdentifierResult(LocalContext &)>;

ExpressionT parseMethodCall(TokenPair &token, const IdentifierFuncT &base);

std::vector<ExpressionT> parseList(TokenPair &token);

FunctionArgumentValues evaluateArgumentList(
    const std::vector<ExpressionT> &args,
    const FunctionArguments &functionArgs,
    LocalContext &context);

void parseAssert(bool condition,
                 const Location &location,
                 std::string_view message = "parsing error") {
    if (!condition) {
        throw VBParsingError{location, std::string{message}};
    }
}

void assertEmpty(TokenPair &token) {
    if (token) {
        throw VBParsingError{token.lastLoc,
                             "Expected end of line, got " + token.content()};
    }
};

Type parseAsStatement(TokenPair &token) {
    auto loc = token.first->loc;
    token.next();

    if (!token.first) {
        throw VBParsingError{loc, "expected typename"};
    }

    auto type = parseType(token.type());

    if (type) {
        token.next();
        return *type;
    }

    // Todo: Make sure to check types in other modules aswell
    if (auto structType = token.structType(token.content())) {
        token.next();
        return Type{Type::Struct, structType};
    }

    if (auto classType = token.classType(token.content())) {
        token.next();
        return Type{Type::Class, classType};
    }

    throw VBParsingError{token.lastLoc, "No such type: " + token.content()};
}

//! For example
//! Sub Hello(There As String, ByRef You as Integer, Optional ByVal Ref)
FunctionArguments parseDeclarationArguments(TokenPair &token) {
    if (!token || token.first->content != "(") {
        throw VBParsingError{token.lastLoc,
                             "expected character '(' got " + token.content()};
    }

    auto args = FunctionArguments{};

    // TODO: Handle Optional
    // https://docs.microsoft.com/en-us/dotnet/visual-basic/programming-guide/language-features/procedures/optional-parameters

    token.next();

    for (bool shouldContinue = true; shouldContinue;) {
        if (token.content() == ")") {
            return args;
        }

        bool byVal = false;

        if (token.type() == Token::ByVal) {
            byVal = true;
            token.next();
        }
        else if (token.type() == Token::ByRef) {
            // Vb6 default
            token.next();
        }

        auto name = token.content();

        token.next();

        if (token.type() != Token::As) {
            throw VBParsingError{token.lastLoc,
                                 "Expected 'As', got " + token.content()};
        }

        auto type = parseAsStatement(token);

        args.push_back({type, name, !byVal});
        token.namedArguments.push_back({name, type});

        if (token.content() == ",") {
            shouldContinue = true;
            token.next();
        }
    }

    return args;
}

const ExpressionT &toExpr(const IdentifierFuncT &i,
                          const LocalContext &context) {
    if (i.index() == 0) {
        return std::get<ExpressionT>(i);
    }

    throw VBRuntimeError{context.currentLocation(),
                         "trying to access non-variable as variable"};
}

IdentifierFuncResult evaluateIdentifier(const IdentifierFuncT &i,
                                        LocalContext &context) {
    if (i.index() == 0) {
        return {std::get<0>(i)(context)};
    }
    else if (i.index() == 0) {
        return {std::get<1>(i)(context)};
    }

    throw VBInternalParsingError{{}, "failed to evaluate identifier"};
}

// LateIdentifierFunc lateMemberAccessor(TokenPair &token, IdentifierFuncT base)
// {
// }

LateIdentifierFunc parseMemberAccessor(TokenPair &token, IdentifierFuncT base) {
    token.next();

    auto name = token.content();
    token.next();

    // We dont know know type at parsing time. The second best is to look up
    // names the first time a command is run
    return [&base, name, loc = token.lastLoc](
               LocalContext &context) -> LateIdentifierResult {
        auto res = LateIdentifierResult{
            .vor = toExpr(base, context)(context),
        };

        auto type = res.vor.get().type();

        if (type.type == Type::Struct) {
            auto memberIndex = type.classType->getVariableIndex(name);

            if (memberIndex == -1) {
                throw VBParsingError{loc, "cannot find member " + name};
            }
            res.f = [base, memberIndex](LocalContext &context) -> ValueOrRef {
                auto &baseVar = toExpr(base, context)(context).get();
                if (baseVar.value.index() != Type::Struct) {
                    throw VBRuntimeError(
                        "trying to access member of non member type line " +
                        std::to_string(context.line));
                }
                auto &s = baseVar.get<StructT>();

                return &s->get(memberIndex);
            };
            return res;
        }
        else if (type.type == Type::Class) {

            auto memberIndex = type.classType->getVariableIndex(name);
            if (memberIndex == -1) {
                throw VBParsingError{loc, "cannot find member " + name};
            }

            // TODO: Handle when there is circular dependencies and the class
            // is unknown on parse time
            res.f = [base, memberIndex](LocalContext &context) -> ValueOrRef {
                auto &baseVar = toExpr(base, context)(context).get();
                if (baseVar.value.index() != Type::Class) {
                    throw VBRuntimeError(
                        "trying to access member of non member type line " +
                        std::to_string(context.line));
                }
                auto &s = baseVar.get<ClassT>();

                return s->get(memberIndex);
            };
            return res;
        }

        throw VBParsingError{
            loc, "cannot access member of non object (ie not struct or class)"};
    };

    //    return lateMemberAccessor(token, base);

    // TODO: Implement class accessor
}

LateIdentifierFunc parseIdentifier(TokenPair &token) {
    auto loc = token.first->loc;
    auto name = token.first->content;

    auto getVarOrFunction = LateIdentifierFunc{};

    auto simpleExpr = ExpressionT{};
    token.next();

    // These first will be known att parse time
    if (auto index = token.localVariable(name); index != -1) {
        simpleExpr = [index](LocalContext &context) -> ValueOrRef {
            return context.localVariables.at(index);
        };
    }
    else if (auto index = token.argument(name); index != -1) {
        simpleExpr = [index](LocalContext &context) -> ValueOrRef {
            return &context.args.at(index).get();
        };
    }
    else if (auto index = token.currentModule->staticVariable(name);
             index != -1) {
        simpleExpr = [index](LocalContext &context) -> ValueOrRef {
            return &context.module->staticVariables.at(index).second;
        };
    }
    else if (token.currentModule->type() == ModuleType::Class) {
        if (auto index = token.currentModule->classType->getVariableIndex(name);
            index != -1) {
            simpleExpr = [index](LocalContext &context) -> ValueOrRef {
                // TODO: Maybe self/me argument should be handled
                // differently...?
                auto &me = context.args.at(0).get();
                if (!me.isClass()) {
                    throw VBInternalParsingError{
                        context.currentLocation(),
                        "Trying to call method on non class "};
                }
                return &me.get<ClassT>()->get(index);
            };
        }
    }
    else if (auto f = token.function(name)) {
        auto base = IdentifierFuncT{};

        // No realtime lookup is necessary
        base = [f](LocalContext &context) -> MethodReferenceReturn {
            return {f};
        };

        //        simpleExpr = [f = parseMethodCall(token, base)](
        //                         LocalContext &context) -> ValueOrRef {
        //            return f(context);
        //        };

        return [base](LocalContext &context) -> LateIdentifierResult {
            return LateIdentifierResult{
                .f = base,
            };
        };
    }

    if (simpleExpr) {
        getVarOrFunction =
            [simpleExpr](LocalContext &context) -> LateIdentifierResult {
            return LateIdentifierResult{
                .vor = simpleExpr(context),
                .f = simpleExpr,
            };
        };
    }

    //    token.next();

    if (!getVarOrFunction) {
        throw VBParsingError{loc, "could not find variable or method " + name};
    }
    else if (token.content() == ".") {

        // Return a function that either connection to right function and then
        // evaluate or just evaluate
        auto baseIdentifierExpr =
            [getVarOrFunction, cachedIdentifierFunc = IdentifierFuncT{}](
                LocalContext &context) mutable -> ValueOrRef {
            if (cachedIdentifierFunc.index() ==
                0) { // Empty result will have index 0
                if (!std::get<0>(cachedIdentifierFunc)) {
                    auto identifier = getVarOrFunction(context);
                    cachedIdentifierFunc = identifier.f;
                    return identifier.vor;
                }

                return std::get<0>(cachedIdentifierFunc)(context);
            }

            throw VBInternalParsingError(context.currentLocation(),
                                         "identifier function");
        };

        getVarOrFunction = parseMemberAccessor(token, baseIdentifierExpr);
    }
    return getVarOrFunction;
}

ExpressionT parseNew(TokenPair &token) {
    token.next();

    if (token.type() != Token::Word) {
        VBParsingError{token.lastLoc,
                       "Expected classname, got other expression: " +
                           token.content()};
    }

    auto name = token.content();

    auto classType = token.classType(name);

    token.next();

    return [classType](LocalContext) -> ValueOrRef {
        return {Value{ClassInstance::create(classType)}};
    };
}

ExpressionT parseExpression(TokenPair &token) {
    auto keyword = token.type();

    ExpressionT expr;

    switch (keyword) {
    case Token::StringLiteral:
        expr = [str = token.first->content](LocalContext &) {
            return ValueOrRef{{str}};
        };
        token.next();
        break;
    case Token::NumberLiteral:
        expr = [l = std::stol(token.first->content)](LocalContext &) {
            return ValueOrRef{{LongT{l}}};
        };
        token.next();
        break;
    case Token::FloatLiteral:
        expr = [i = std::stod(token.first->content)](LocalContext &) {
            return ValueOrRef{{DoubleT{i}}};
        };
        token.next();
        break;

    case Token::Word: {
        auto idFunction = parseIdentifier(token);
        expr = [idFunction, f = ExpressionT{}](
                   LocalContext &context) mutable -> ValueOrRef {
            if (!f) {
                auto res = idFunction(context);
                f = std::get<ExpressionT>(res.f);
            }

            return f(context); //
        };
    } break;

    case Token::New:
        expr = parseNew(token);
        break;
    default:
        throw VBParsingError{token.lastLoc,
                             "unexpected token " + token.content()};
    };

    if (!expr) {
        throw VBParsingError{*token.first, "failied to parse expression"};
    }

    //    token.next();

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

//! Evaluate function arguments in runtime
//! @arg functionArgs is used to get information about for example ByVal
FunctionArgumentValues evaluateArgumentList(
    const std::vector<ExpressionT> &args,
    const FunctionArguments &functionArgs,
    LocalContext &context) {
    auto values = FunctionArgumentValues{};
    values.reserve(args.size());

    if (functionArgs.size() != args.size()) {
        throw "this should not happend.. yet";
    }

    for (size_t i = 0; i < args.size(); ++i) {
        auto &arg = args.at(i);
        auto &funcArg = functionArgs.at(i);

        if (funcArg.isByRef) {
            values.push_back(arg(context));
        }
        else {
            values.push_back(ValueOrRef{arg(context).get()});
        }
    }

    return values;
}

ExpressionT parseMethodCall(TokenPair &token, const IdentifierFuncT &base) {
    auto argExpressionList = parseList(token);

    assertEmpty(token);

    return [base,
            argExpressionList,
            function = static_cast<Function *>(nullptr),
            loc = token.lastLoc](LocalContext &context) mutable {
        if (!function) {
            function = std::get<1>(base)(context).f;

            if (!function) {
                VBRuntimeError{context.currentLocation(),
                               "could not find function "};
            }
            else {
                if (argExpressionList.size() > function->arguments().size()) {
                    VBParsingError{loc, "to many argument for function"};
                }
            }
        }

        auto args = evaluateArgumentList(
            argExpressionList, function->arguments(), context);

        return function->call(args, context);
    };
}

void parseLocalVariableDeclaration(TokenPair &token) {
    if (!token.currentFunctionBody) {
        throw VBInternalParsingError{*token.first, "no function body"};
    }

    auto name = token.first->content;

    token.next();

    if (!(token && token.type() == Token::As)) {
        throw VBParsingError{token.lastLoc,
                             "Expected 'As' statement got " + token.content()};
    }

    auto type = parseAsStatement(token);

    token.namedLocalVariables.push_back(name);

    token.currentFunctionBody->pushLocalVariable(type);
}

void parseMemberDeclaration(TokenPair &token) {
    auto loc = token.first->loc;
    if (token.type() != Token::Word) {
        token.next();
        parseAssert(token, loc, "Expected variable name");
    }

    auto name = token.content();

    token.next();
    if (token.type() != Token::As) {

        throw VBParsingError{token.lastLoc,
                             "Expected 'As' statement got " + token.content()};
    }

    auto type = parseAsStatement(token);

    // TODO: Handle differences between class and modules where modules
    // always have global/static variables and classes as standard have
    // nonstatic variables
    if (token.currentModule->type() == ModuleType::Class) {
        auto classType = token.currentModule->classType.get();

        classType->addAddVariable(name, type);
    }
    else {
        token.currentModule->staticVariables.push_back(
            {name, Value::create(type)});
    }
}

void parseMemberDeclarationList(TokenPair &token) {
    parseMemberDeclaration(token);

    // TODO: Loop for all declarations on line
}

FunctionBody::CommandT parseAssignment(TokenPair &token, ExpressionT target) {
    auto loc = token.first->loc;
    token.next();

    if (!token) {
        throw VBParsingError{loc, "Expected expression to assign to"};
    }

    auto expr = parseExpression(token);

    return [target, expr](LocalContext &context) {
        target(context) = expr(context).get();
    };
    //    return [target, expr](LocalContext &context) {
    //        toExpr(target, context)(context) = expr(context).get();
    //    };
}

//! A command is a line inside a function that starts at the beginning of
//! the line
FunctionBody::CommandT parseCommand(TokenPair &token) {
    switch (token.type()) {
    case Token::Print: {
        parseAssert(token.second, *token.first, "expected expression");
        token.next();
        auto expr = parseExpression(token);

        assertEmpty(token);
        return [expr](LocalContext &context) {
            std::cout << expr(context).get().toString() << std::endl; //
        };
    } break;

    case Token::Dim:
        token.next();
        parseLocalVariableDeclaration(token);

        assertEmpty(token);
        break;

    case Token::Word: {
        {
            auto identifier = parseIdentifier(token);
            if (token.content() == "=") {
                auto expr = [identifier, f = ExpressionT{}](
                                LocalContext &context) mutable -> ValueOrRef {
                    if (!f) {
                        auto res = identifier(context);
                        f = std::get<0>(res.f);
                        return res.vor;
                    }

                    return f(context);
                };

                return parseAssignment(token, expr);
            }
            else if (auto t = token.type();
                     t == Token::Word || t == Token::NumberLiteral) {
                // Method calls
                // For example
                // ExampleMethod "Hello"

                auto argExpr = parseList(token);
                auto expr = [identifier, f = MethodReferenceReturn{}, argExpr](
                                LocalContext &context) mutable -> ValueOrRef {
                    if (!f) {
                        auto res = identifier(context);
                        f = std::get<1>(res.f)(context);
                    }

                    auto classFunction = f.f;

                    return classFunction->call(
                        evaluateArgumentList(
                            argExpr, classFunction->arguments(), context),
                        context);
                };

                return expr;
            }
            else {
                throw VBParsingError{token.lastLoc,
                                     "unexpected reserved keyword " +
                                         token.content()};
            }
            break;
        }

        //        auto type2 = token.second ? token.second->type() :
        //        Token::Empty; if (type2 == Token::Operator ||
        //            (token.second && token.second->content == ".")) {
        //            // TODO: Fix this
        //            auto identifier = parseIdentifier(token);
        //            return parseAssignment(token, identifier);
        //        }
        //        else {
        //            return parseMethodCall(token);
        //        }
    }
    default:

        throw VBParsingError{*token.first,
                             "unexpected command " + token.first->content};
    }

    return {};
}

void parseStruct(Line *line,
                 Scope scope,
                 TokenPair &token,
                 NextLineT nextLine) {
    token.next();
    auto name = token.content();

    auto &structType = token.currentModule->addStruct(name);

    for (line = nextLine(); line; line = nextLine()) {
        token.next();
        auto keyword = token.type();
        if (keyword == Token::End) {
            break;
        }

        if (token.type() != Token::Word) {
            throw VBParsingError{token.lastLoc,
                                 "Unexpected token " + token.content()};
        }

        auto memberName = token.content();

        token.next();
        if (token.type() != Token::As) {
            throw VBParsingError{token.lastLoc,
                                 "Expected As got " + token.content()};
        }

        auto type = parseAsStatement(token);

        structType.addAddVariable(memberName, type);
    }
}

std::unique_ptr<Function> parseFunction(Line *line,
                                        Scope scope,
                                        TokenPair &token,
                                        NextLineT nextLine) {

    token.next();

    auto name = token.content();

    token.next();

    auto arguments = parseDeclarationArguments(token);

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
        body->pushCommand(parseCommand(token), token.lastLoc.line);
    }

    Function::FuncT f = [body](const FunctionArgumentValues &args,
                               LocalContext &context) -> Value {
        return body->call(args, context);
    };

    auto function = std::make_unique<Function>(name, std::move(arguments), f);

    token.endFunction();

    return function;
}

std::unique_ptr<Module> parseGlobal(Line *line,
                                    NextTokenT nextToken,
                                    NextLineT nextLine,
                                    std::filesystem::path path,
                                    const ModuleList &moduleList) {
    auto module = std::make_unique<Module>();
    module->path = path;
    if (path.extension() == ".cls") {
        module->classType = std::make_unique<ClassType>();
        module->classType->module = module.get();
        module->classType->name = path.stem();
    }

    Scope currentScope;

    for (; line; line = nextLine()) {
        currentScope = Private;
        TokenPair token{};
        token.f = nextToken;
        token.currentModule = module.get();
        token.next();
        token.modules = moduleList; // Copy on every line... :)

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
            module->addFunction(parseFunction(line, Private, token, nextLine));
            continue;
            break;
        case Token::Option:
            continue; // Skip option statements, assume Option Explicit
        case Token::Dim:
            parseMemberDeclaration(token);
            continue;
        case Token::Structure:
            parseStruct(line, currentScope, token, nextLine);
            continue;
        default:
            throw VBParsingError{*token.first,
                                 "unexpected token at global scope: " +
                                     token.content()};
        }

        token.next();

        if (!token) {
            throw VBParsingError{token.lastLoc, "Unexpected end of line"};
            continue;
        }

        switch (token.first->type()) {
        case Token::Sub:
            module->addFunction(
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

std::unique_ptr<Module> parse(std::istream &stream,
                              std::filesystem::path path,
                              const ModuleList &moduleList) {

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

    return parseGlobal(line, nextToken, nextLine, path, moduleList);
}

std::unique_ptr<Module> loadModule(std::filesystem::path path,
                                   const ModuleList &moduleList) {
    auto file = std::ifstream{path};
    return parse(file, path, moduleList);
}
