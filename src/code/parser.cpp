#include "parser.h"
#include "codeblock.h"
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
using NextLineT = std::function<Line *()>;
using ExpressionT = std::function<ValueOrRef(LocalContext &)>;
using IdentifierFuncT = std::function<ValueOrRef(LocalContext &context)>;

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

    bool isWithStatement;

    void endFunction() {
        currentFunctionBody = nullptr;
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

ExpressionT parseExpression(TokenPair &token);

[[nodiscard]] FunctionBody::CommandT parseBlock(
    Line **line,
    TokenPair &token,
    NextLineT nextLine,
    std::function<bool(Token::Keyword)> endCondition,
    bool shouldConsumeEnd = true);

[[nodiscard]] IdentifierFuncT parseWithAccessor(TokenPair &token);

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
FunctionArguments parseArguments(TokenPair &token) {
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

        if (token.content() == ",") {
            shouldContinue = true;
            token.next();
        }
    }

    return args;
}

IdentifierFuncT parseMemberAccessor(TokenPair &token, IdentifierFuncT base) {
    token.next();

    auto name = token.content();
    auto loc = token.lastLoc;
    token.next();

    return [base, name, loc](LocalContext &context) -> ValueOrRef {
        // TODO: handle Object type (when that is added)

        auto baseVar = base(context);
        auto type = baseVar->type();

        if (!type.classType) {
            throw VBParsingError{
                loc,
                "cannot access member of non object (ie not struct or class)"};
        }

        // TODO: cache member index
        auto memberIndex = type.classType->getVariableIndex(name);
        if (memberIndex != -1) {

            if (type.type != Type::Struct && type.type != Type::Class) {
                throw VBParsingError{loc,
                                     "cannot access member of non object (ie "
                                     "not struct or class)"};
            }

            auto s = type.type == Type::Struct ? baseVar->get<StructT>().get()
                                               : baseVar->get<ClassT>().get();

            return {&s->get(memberIndex)};
        }

        // TODO: Cache function
        auto memberFunction = type.classType->module->function(name);
        if (memberFunction) {
            return FunctionRef{memberFunction, baseVar->get<ClassT>()};
        }

        throw VBParsingError{context.currentLocation(),
                             "cannot find member " + name};
    };
}

IdentifierFuncT getLocalIdentifier(std::string_view name, FunctionBody *body) {
    if (auto index = body->variableIndex(name); index != -1) {
        return [index](LocalContext &context) -> ValueOrRef {
            return {&context.localVariables.at(index)};
        };
    }

    return {};
}

IdentifierFuncT parseIdentifier(TokenPair &token) {
    auto loc = token.first->loc;
    auto name = token.content();

    if (token.type() == Token::Period) {
        return parseWithAccessor(token);
    }

    token.next();

    auto expr = IdentifierFuncT{};

    // Local variables are special, we can always know if they exist
    // And this is important to achieve scope for variables
    // TODO: Do this for function argumens aswell. But then we need to change so
    // that the definitions of arguments is added at the beginning of
    // parseFunction() and not in the end as of now
    if (auto f = getLocalIdentifier(name, token.currentFunctionBody)) {
        expr = f;
    }
    else {
        // Runtime lookup of variable names
        expr = [name](LocalContext &context) -> ValueOrRef {
            if (auto index =
                    context.functionBody->function()->argumentIndex(name);
                index != -1) {
                return {&context.args.at(index).get()};
            }
            else if (auto index = context.module->staticVariable(name);
                     index != -1) {
                return {&context.module->staticVariables.at(index).second};
            }
            else if (auto f = context.function(name)) {
                // TODO: Cache function pointer
                return FunctionRef{f};
            }
            else if (name == "Me") {
                if (context.me.typeName() != Type::Class) {
                    throw VBParsingError{
                        context.currentLocation(),
                        "Trying to access 'Me' in static context " + name};
                }
                return {context.me};
            }
            else if (auto me = context.me.toClass()) {
                if (auto index = me->getMemberIndex(name); index != -1) {
                    return {&me->get(index)};
                }
            }

            throw VBParsingError{context.currentLocation(),
                                 "could not find variable " + name};
        };
    }

    if (token.content() != ".") {
        return expr;
    }

    return parseMemberAccessor(token, expr);
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

template <typename T>
std::function<T(T, T)> getOperator(std::string name) {

#define DEFINE_BINARY_OPERATOR(x)                                              \
    {                                                                          \
#x, [](T first, T second) -> T { return first x second; }              \
    }

    static const auto ops = std::map<std::string, std::function<T(T, T)>>{
        DEFINE_BINARY_OPERATOR(+),
        DEFINE_BINARY_OPERATOR(-),
        DEFINE_BINARY_OPERATOR(/),
        DEFINE_BINARY_OPERATOR(*),
        DEFINE_BINARY_OPERATOR(<),
        DEFINE_BINARY_OPERATOR(>),
        {"=", [](T first, T second) -> T { return first == second; }},
        {"<>", [](T first, T second) -> T { return first != second; }},
    };

    if (auto f = ops.find(name); f != ops.end()) {
        return f->second;
    }

#undef DEFINE_BINARY_OPERATOR

    throw VBRuntimeError{"binary operator not implemented: " + name};
}

ExpressionT parseBinary(ExpressionT first, TokenPair &token) {
    auto op = token.content();

    token.next();
    auto second = parseExpression(token);

    auto opFInt = getOperator<LongT>(op);
    auto opFFloat = getOperator<DoubleT>(op);

    return
        [first, second, opFInt, opFFloat](LocalContext &context) -> ValueOrRef {
            auto firstResult = first(context);
            auto secondResult = second(context);
            auto type = firstResult->commonType(secondResult->type());

            switch (type.type) {
            case Type::Integer:
            case Type::Long:
                return Value{opFInt(firstResult->toInteger(),
                                    secondResult->toInteger())};
            case Type::Double:
            case Type::Single:
                return Value{
                    opFFloat(firstResult->toFloat(), secondResult->toFloat())};
                break;
            default:
                break;
            }

            throw VBRuntimeError{context.currentLocation(),
                                 "could not convert to numeric value"};
        };
}

ExpressionT parseExpression(TokenPair &token) {
    auto keyword = token.type();

    ExpressionT expr;

    auto name = token.content();

    switch (keyword) {
    case Token::StringLiteral:
        token.next();
        expr = [name](LocalContext &) { return ValueOrRef{std::string{name}}; };
        break;
    case Token::NumberLiteral:
        token.next();
        expr = [l = std::stol(name)](LocalContext &) {
            return ValueOrRef{{LongT{l}}};
        };
        break;
    case Token::FloatLiteral:
        token.next();
        expr = [i = std::stod(name)](LocalContext &) {
            return ValueOrRef{{DoubleT{i}}};
        };
        break;

    case Token::Me:
    case Token::Period:
    case Token::Word: {
        auto id = parseIdentifier(token);
        expr = id;
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

    if (!token) {
        return expr;
    }

    switch (token.type()) {
    case Token::Operator:
        return parseBinary(expr, token);
        break;
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

FunctionBody::CommandT parseMethodCall(TokenPair &token,
                                       ExpressionT functionExpression) {
    auto argExpressionList = parseList(token);

    assertEmpty(token);

    return [functionExpression,
            argExpressionList,
            function = static_cast<const Function *>(nullptr),
            loc = token.lastLoc](LocalContext &context) mutable -> ReturnT {
        auto ref = functionExpression(context).function();
        function = ref.get();
        if (!function) {
            VBRuntimeError{"could not find function"};
        }
        if (argExpressionList.size() > function->arguments().size()) {
            VBParsingError{loc,
                           "to many argument for function " +
                               std::string{function->name()}};
        }

        // TODO: Cache static function calls

        auto args = evaluateArgumentList(
            argExpressionList, function->arguments(), context);

        function->call(args, ref.me, context);

        return ReturnT::Standard;
    };
}

CommandT parseExitStatement(TokenPair &token) {
    token.next();

    switch (token.type()) {
    case Token::For:
        return [](LocalContext) { return ReturnT::ExitFor; };
    default:
        throw VBParsingError{token.lastLoc,
                             "Unexpected token (expected Do | For | Function | "
                             "Property | Select | Sub | Try | While) " +
                                 token.content()};
    }
}

CommandT parseContinue(TokenPair &token) {
    token.next();

    switch (token.type()) {
    case Token::For:
        return [](LocalContext) { return ReturnT::ContinueFor; };
    default:
        throw VBParsingError{token.lastLoc,
                             "Unexpected token (expected Do | For | While) " +
                                 token.content()};
    }
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

    try {
        token.currentFunctionBody->pushLocalVariable(name, type);
    }
    catch (std::logic_error &e) {
        throw VBParsingError{token.lastLoc,
                             "Variable " + name + " already exists"};
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
    if (token.type() != Token::As) {

        throw VBParsingError{token.lastLoc,
                             "Expected 'As' statement got " + token.content()};
    }

    auto type = parseAsStatement(token);

    // TODO: Handle differences between class and modules where modules always
    // have global/static variables and classes as standard have nonstatic
    // variables
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

FunctionBody::CommandT parseAssignment(TokenPair &token,
                                       IdentifierFuncT target,
                                       bool isSetStatement = false) {
    auto loc = token.first->loc;

    if (token.content() != "=") {
        throw VBParsingError{loc, "expected '=' got " + token.content()};
    }
    token.next();

    if (!token) {
        throw VBParsingError{loc, "Expected expression to assign to"};
    }

    auto expr = parseExpression(token);

    assertEmpty(token);

    return [target, expr, isSetStatement](LocalContext &context) {
        auto t = target(context);
        if ((t->typeName() == Type::Class) != isSetStatement) {
            throw VBRuntimeError{context.currentLocation(),
                                 isSetStatement
                                     ? "using set statement on non object/class"
                                     : "missing set statement on object/class"};
        }
        *t = *expr(context);
        return ReturnT::Standard;
    };
}

//! A command is a line inside a function that starts at the beginning of the
//! line
FunctionBody::CommandT parseCommand(TokenPair &token) {
    switch (token.type()) {
    case Token::Print: {
        parseAssert(token.second, *token.first, "expected expression");
        token.next();
        auto expr = parseExpression(token);

        assertEmpty(token);
        return [expr](LocalContext &context) {
            std::cout << expr(context).get().toString() << std::endl; //
            return ReturnT::Standard;
        };
    } break;

    case Token::Dim:
        token.next();
        parseLocalVariableDeclaration(token);

        assertEmpty(token);
        break;
    case Token::Exit:
        return parseExitStatement(token);
    case Token::Continue:
        return parseContinue(token);
    case Token::Set: {
        token.next();
        return parseAssignment(token, parseIdentifier(token), true);
    }

    case Token::Period:
    case Token::Me:
    case Token::Word: {
        {
            auto identifier = parseIdentifier(token);

            if (token.content() == "=") {
                return parseAssignment(token, identifier);
            }
            else {
                return parseMethodCall(token, identifier);
            }
        }
    }
    case Token::Empty:
        throw VBParsingError{*token.first,
                             "unexpected end of line " + token.content()};
    default:
        throw VBParsingError{*token.first,
                             "unexpected command " + token.content()};
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

    nextLine();
}

FunctionBody::CommandT parseSelectCase(Line **line,
                                       TokenPair &token,
                                       NextLineT nextLine) {
    throw VBParsingError{token.lastLoc, "select case not implemented"};
}

FunctionBody::CommandT parseIfStatement(Line **line,
                                        TokenPair &token,
                                        NextLineT nextLine) {
    auto codeBlock = FunctionBody::CommandT{};

    FunctionBody::CommandT elseStatement = {};

    auto isRoot = token.type() == Token::If;
    auto isElse = token.type() == Token::Else;

    token.next();
    auto condition = [&]() -> ExpressionT {
        if (isElse) {
            return [](LocalContext &context) -> Value { return true; };
        }
        else {
            return parseExpression(token);
        }
    }();

    if (false) { // If pedantic...
        if (!isElse) {
            if (token.type() != Token::Then) {
                throw VBParsingError{token.lastLoc,
                                     "Expected 'Then' after If-statement"};
            }
        }
    }

    codeBlock = parseBlock(
        line,
        token,
        nextLine,
        [](auto t) {
            return t == Token::End || t == Token::ElseIf || t == Token::Else;
        },
        false);

    if (token.type() != Token::End) {
        elseStatement = parseIfStatement(line, token, nextLine);
    }

    if (isRoot) {
        *line = nextLine();
    }

    return [condition, codeBlock, elseStatement](LocalContext &context) {
        if (condition(context)->toBool()) {
            return codeBlock(context);
        }
        else if (elseStatement) {
            return elseStatement(context);
        }
        return ReturnT::Standard;
    };
}

FunctionBody::CommandT parseForStatement(Line **line,
                                         TokenPair &token,
                                         NextLineT nextLine) {

    token.next();

    if (token.type() == Token::Each) {
        // TODO: For or for each
        token.next();
        auto name = token.content();
        token.next();
        auto type = std::optional<Type>{};

        if (token.type() == Token::As) {
            type = parseAsStatement(token);
        }

        if (token.type() != Token::In) {
            throw VBParsingError{token.lastLoc,
                                 "Expected 'In' got " + token.content()};
        }
        token.next();

        auto container = parseIdentifier(token);
        auto codeBlock = parseBlock(
            line, token, nextLine, [](auto t) { return t == Token::Next; });

        // TODO: Handle variable declaration

        return [codeBlock](LocalContext &context) {
            std::cerr << "for each not implemented yet" << std::endl;
            return ReturnT::Standard;
        };
    }

    auto name = token.content();

    token.next();

    auto isScopeLocalVariable = false;

    if (token.type() == Token::As) {
        isScopeLocalVariable = true;
        auto type = parseAsStatement(token);

        token.currentFunctionBody->pushLocalVariable(name, type);
    }

    auto variableIdentification =
        getLocalIdentifier(name, token.currentFunctionBody);

    if (!variableIdentification) {
        throw VBParsingError{token.lastLoc, "Could not find variable " + name};
    }

    if (token.content() != "=") {
        throw VBParsingError{token.lastLoc,
                             "Expected '=' got " + token.content()};
    }

    token.next();

    auto start = parseExpression(token);

    if (token.type() != Token::To) {
        throw VBParsingError{token.lastLoc,
                             "Expected 'To' got " + token.content()};
    }
    token.next();

    auto stop = parseExpression(token);

    auto step =
        ExpressionT{[](LocalContext &) -> ValueOrRef { return Value{1}; }};

    if (token.type() == Token::Step) {
        token.next();
        step = parseExpression(token);
    }

    auto code = parseBlock(
        line, token, nextLine, [](auto t) { return t == Token::Next; });

    if (isScopeLocalVariable) {
        token.currentFunctionBody->forgetLocalVariableName(name);
    }

    return [start, code, stop, step, variableIdentification](
               LocalContext &context) {
        auto var = variableIdentification(context);
        auto ret = ReturnT::Standard;
        for (var.get() = *start(context);
             var.get().toInteger() <= stop(context)->toInteger();
             var.get() = var->toInteger() + step(context)->toInteger()) {
            ret = code(context);
            if (ret == ReturnT::ExitFor) {
                return ReturnT::Standard;
                break;
            }
            else if (ret == ReturnT::ContinueFor) {
                continue;
            }
            else if (ret != ReturnT::Standard) {
                return ret;
            }
        }
        return ReturnT::Standard;
    };
    throw VBParsingError{token.lastLoc, "for loops not implementede yet"};
}

CommandT parseWith(TokenPair &token) {
    token.next();
    auto identifier = parseIdentifier(token);
    token.isWithStatement = true;
    return [identifier](LocalContext &context) {
        context.with = identifier(context);
        return ReturnT::Standard;
    };
}

//! The stuff that is inside a With-statement
//! For example
//! With X
//!     .X = 1  <-- this
//! End With
IdentifierFuncT parseWithAccessor(TokenPair &token) {
    auto withIdentifier = [](LocalContext &context) -> ValueOrRef {
        return context.with;
    };
    return parseMemberAccessor(token, withIdentifier);
}

FunctionBody::CommandT parseBlock(
    Line **line,
    TokenPair &token,
    NextLineT nextLine,
    std::function<bool(Token::Keyword)> endCondition,
    bool consumeEnd) {

    auto block = CodeBlock{};

    for (*line = nextLine(); *line;) {
        token.next();
        auto t = token.type();

        if (t == Token::End) {
            if (token.second && token.second->type() == Token::With) {
                if (token.isWithStatement) {
                    token.isWithStatement = false;
                    *line = nextLine();
                    continue;
                }
                else {
                    throw VBParsingError{
                        token.lastLoc,
                        "'End With' without matching 'With ...'-statement"};
                }
            }
        }

        if (endCondition(t)) {
            if (consumeEnd) {
                *line = nextLine();
            }
            return //
                [block = std::move(block)](LocalContext &context) {
                    return block.run(context);
                };
        }

        if (t == Token::If) {
            block.addCommand(parseIfStatement(line, token, nextLine));
        }
        else if (t == Token::For) {
            block.addCommand(parseForStatement(line, token, nextLine));
        }
        else if (t == Token::Select) {
            block.addCommand(parseSelectCase(line, token, nextLine));
        }
        else if (t == Token::With) {
            // TODO: Handle this
            block.addCommand(parseWith(token));
            *line = nextLine();
        }
        else {
            // TODO: Move line number into the command type
            if (auto command = parseCommand(token)) {
                block.addCommand(std::move(command));
            }
            *line = nextLine();
        }
    }

    throw VBParsingError{token.lastLoc,
                         "Expected end of block, but got end of file"};
}

std::unique_ptr<Function> parseFunction(Line **line,
                                        Scope scope,
                                        TokenPair &token,
                                        NextLineT nextLine) {

    token.next();
    auto name = token.content();
    token.next();
    auto arguments = parseArguments(token);

    bool isStatic = static_cast<bool>(token.currentModule->classType);
    auto body = std::make_shared<FunctionBody>();

    token.currentFunctionBody = body.get();

    body->pushCommand(parseBlock(line,
                                 token,
                                 nextLine,
                                 [](Token::Keyword t) {
                                     return t == Token::End; //
                                 }),
                      token.lastLoc.line);

    Function::FuncT f = [body](const FunctionArgumentValues &args,
                               Value me,
                               LocalContext &context) -> Value {
        return body->call(args, me, context);
    };

    auto function =
        std::make_unique<Function>(name, std::move(arguments), f, isStatic);
    body->function(function.get());

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

    for (; line;) {
        currentScope = Private;
        TokenPair token{};
        token.f = nextToken;
        token.currentModule = module.get();
        token.next();
        token.modules = moduleList; // Copy on every line... :)

        if (!token) {
            line = nextLine();
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
            module->addFunction(parseFunction(&line, Private, token, nextLine));
            continue;
            break;
        case Token::Option:
            line = nextLine();
            continue; // Skip option statements, assume Option Explicit
        case Token::Dim:
            parseMemberDeclaration(token);
            line = nextLine();
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
                parseFunction(&line, currentScope, token, nextLine));
            break;

        case Token::Word:
            parseMemberDeclaration(token);
            line = nextLine();
            break;

        case Token::Structure:
            parseStruct(line, currentScope, token, nextLine);
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
