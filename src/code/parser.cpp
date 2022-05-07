#include "parser.h"
#include "classinstance.h"
#include "classtype.h"
#include "codeblock.h"
#include "engine/function.h"
#include "functionbody.h"
#include "lexer.h"
#include "parsetypes.h"
#include "stringutils.h"
#include "vbparsingerror.h"
#include <cmath>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>

namespace {

enum Scope {
    Public,
    Private,
    None,
};

using NextTokenT = std::function<std::pair<Token *, Token *>()>;
using NextLineT = std::function<Line *()>;
using ExpressionT = std::function<ValueOrRef(Context &)>;
using IdentifierFuncT = std::function<ValueOrRef(Context &context)>;

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
    const std::vector<std::shared_ptr<Module>> *modules;
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

        for (auto &m : *modules) {
            if (auto t = m->structType(name)) {
                return t;
            }
        }

        return nullptr;
    }

    ClassType *classType(std::string_view name) const {
        for (auto &m : *modules) {
            if (m->type() == ModuleType::Class && iequals(m->name(), name)) {
                return m->classType.get();
            }
        }

        return nullptr;
    }

    Function *function(std::string_view name) const {
        for (auto &m : *modules) {
            if (m->type() == ModuleType::Module) {
                if (auto f = m->function(name)) {
                    return f;
                }
            }
        }
        return nullptr;
    }
};

ExpressionT parseExpression(TokenPair &token, bool ignoreBinary = false);

[[nodiscard]] FunctionBody::CommandT parseBlock(
    Line **line,
    TokenPair &token,
    NextLineT nextLine,
    std::function<bool(Token::Keyword)> endCondition,
    bool shouldConsumeEnd = true);

[[nodiscard]] IdentifierFuncT parseWithAccessor(TokenPair &token);

[[nodiscard]] ExpressionT parseFunctionCall(TokenPair &token,
                                            ExpressionT functionExpression);

[[nodiscard]] ExpressionT parseBinary(ExpressionT first,
                                      TokenPair &token,
                                      int previousPrecedence = 1000);

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

void assertEqual(TokenPair &token,
                 Token::Keyword type,
                 const std::string &shownName = "") {
    if (token.type() != type) {
        if (shownName.empty()) {
            throw VBParsingError{token.lastLoc,
                                 "Unexpected token " + token.content()};
        }
        else {
            throw VBParsingError{token.lastLoc,
                                 "Expected '" + shownName + "' got " +
                                     token.content()};
        }
    }
}

constexpr auto shorthandCharacters = std::string_view{"%&@!#$"};
constexpr static auto shorthandTypes = std::array{
    Type::Integer,
    Type::Long,
    Type::Integer, // TODO: Decimial, but not implemented yet
    Type::Single,
    Type::Double,
    Type::String,
};

bool isShorthandType(char c) {
    return shorthandCharacters.find(c) != std::string::npos;
}

Type::TypeName parseShorthandType(TokenPair &token) {

    auto content = token.content();
    if (content.size() != 1) {
        throw VBParsingError{token.lastLoc, "unknown type " + token.content()};
    }
    auto f = shorthandCharacters.find(content.front());

    if (f == std::string::npos) {
        throw VBParsingError{token.lastLoc, "unknown type " + token.content()};
    }

    token.next();

    return shorthandTypes.at(f);
}

//! 0 -> Not type declaration
//! 1 -> As statement
//! 2 -> Shorthand type notation
int isTypeDeclaration(TokenPair &token) {
    if (token.type() == Token::As) {
        return 1;
    }
    if (token.type() == Token::Operator) {
        auto content = token.content();
        if (content.size() != 1) {
            return 0;
        }
        if (isShorthandType(content.front())) {
            return 2;
        }
    }
    return 0;
}

int assertTypeDeclaration(TokenPair &token) {
    if (auto ret = isTypeDeclaration(token)) {
        return ret;
    }
    throw VBParsingError{
        token.lastLoc,
        "Expected type specification for example 'As Integer'  got " +
            token.content()};
}

TypeDescription parseAsStatement(TokenPair &token) {
    auto loc = token.first->loc;

    auto typeSyntax = assertTypeDeclaration(token);

    if (typeSyntax == 2) {
        return TypeDescription{Type{parseShorthandType(token)}};
    }

    token.next();

    if (!token.first) {
        throw VBParsingError{loc, "expected typename"};
    }

    bool shouldCreateNew = false;

    if (token.type() == Token::New) {
        shouldCreateNew = true;
        token.next();
    }

    auto type = parseType(token.type());

    if (type) {
        token.next();
        return {*type};
    }

    // Todo: Make sure to check types in other modules aswell
    if (auto structType = token.structType(token.content())) {
        token.next();
        return {Type{Type::Struct, structType}};
    }

    if (auto classType = token.classType(token.content())) {
        token.next();
        return {Type{Type::Class, classType}, shouldCreateNew};
    }

    if (auto c = token.currentModule->classType.get()) {
        if (iequals(c->name, token.content())) {
            token.next();
            return {Type{Type::Class, c}, shouldCreateNew};
        }
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
        bool isOptional = false;

        if (token.type() == Token::Optional) {
            isOptional = true;
            token.next();
        }

        if (token.type() == Token::ByVal) {
            byVal = true;
            token.next();
        }
        else if (token.type() == Token::ByRef) {
            // Vb6 default, just continue to next
            token.next();
        }

        auto name = token.content();

        token.next();

        auto type = parseAsStatement(token);

        auto optionalValue = ExpressionT{};

        if (isOptional && token.content() == "=") {
            token.next();
            optionalValue = parseExpression(token);
        }

        args.push_back(FunctionArgument{
            .type = type.type,
            .name = name,
            .isByRef = !byVal,
            .shouldCreateNew = type.shouldCreateNew,
            .defaultValue = optionalValue,
        });

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

    return [base, name, loc](Context &context) -> ValueOrRef {
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

            auto s = (type.type == Type::Struct) ? baseVar->get<StructT>().get()
                                                 : baseVar->get<ClassT>().get();

            return ValueOrRef{&s->get(memberIndex)};
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
        return [index](Context &context) -> ValueOrRef {
            return {&context.localVariables.at(index)};
        };
    }

    if (body->function()->name() == name) {
        return [](Context &context) -> ValueOrRef {
            return {&context.returnValue};
        };
    }

    return {};
}

IdentifierFuncT parseIdentifier(TokenPair &token) {
    auto loc = token.first->loc;
    auto name = token.content();

    if (token.type() != Token::Word && token.type() != Token::Me &&
        token.type() != Token::Period && token.type() != Token::MsgBox) {
        throw VBParsingError{token.lastLoc,
                             "Expected word got " + token.content()};
    }

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
        expr = [name](Context &context) -> ValueOrRef {
            if (auto index =
                    context.functionBody->function()->argumentIndex(name);
                index != -1) {
                return {&context.args.at(index).get()};
            }
            else if (auto index = context.module->staticVariableIndex(name);
                     index != -1) {
                return {&context.module->staticVariable(index)};
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

    while (token.type() == Token::Period) {
        expr = parseMemberAccessor(token, expr);
    }

    return expr;
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

    return [classType](Context) -> ValueOrRef {
        return {Value{ClassInstance::create(classType)}};
    };
}

template <typename T>
std::pair<std::function<T(T, T)>, int> getOperator(std::string name) {

#define DEFINE_BINARY_OPERATOR(x, precedence)                                  \
    {                                                                          \
#x, {                                                                  \
            [](T first, T second) -> T { return first x second; }, precedence  \
        }                                                                      \
    }

    //! Precedence
    //! https://docs.microsoft.com/en-us/dotnet/visual-basic/language-reference/operators/operator-precedence

    static const auto ops = std::map<std::string,
                                     std::pair<std::function<T(T, T)>, int>>{
        {"^",
         {[](T first, T second) -> T { return std::pow(first, second); }, 0}},
        // Unary +- -> 1
        DEFINE_BINARY_OPERATOR(/, 2),
        DEFINE_BINARY_OPERATOR(*, 2),
        {"\\",
         {[](T first, T second) -> T {
              return std::round(static_cast<double>(first) / second);
          },
          3}},
        {"Mod",
         {[](T first, T second) -> T {
              return std::round(static_cast<LongT>(first) %
                                static_cast<LongT>(second));
          },
          4}},
        DEFINE_BINARY_OPERATOR(+, 5),
        DEFINE_BINARY_OPERATOR(-, 5),
        // Here should string concatenation be
        {"<<",
         {[](T first, T second) -> T {
              return std::round(static_cast<LongT>(first)
                                << static_cast<LongT>(second));
          },
          7}},
        {">>",
         {[](T first, T second) -> T {
              return std::round(static_cast<LongT>(first) >>
                                static_cast<LongT>(second));
          },
          7}},
        DEFINE_BINARY_OPERATOR(<, 8),
        DEFINE_BINARY_OPERATOR(>, 8),
        DEFINE_BINARY_OPERATOR(<=, 8),
        DEFINE_BINARY_OPERATOR(>=, 8),
        {"=", {[](T first, T second) -> T { return first == second; }, 8}},
        {"<>", {[](T first, T second) -> T { return first != second; }, 8}},
        {"Is",
         {[](T first, T second) -> T {
              throw VBRuntimeError{"Trying to use 'Is' on integer values "};
          },
          8}},
        //            {"Not", {[](T first, T second) -> T { return first !=
        //            second; }, 9}},
        {"And", {[](T first, T second) -> T { return first && second; }, 10}},
        // AndAlso
        {"Or", {[](T first, T second) -> T { return first || second; }, 11}},
        {"Xor", {[](T first, T second) -> T { return first != second; }, 12}},
    };

    if (auto f = ops.find(name); f != ops.end()) {
        return f->second;
    }

    throw VBRuntimeError{"binary operator not implemented: " + name};
#undef DEFINE_BINARY_OPERATOR
}

ExpressionT parseUnary(TokenPair &token, int previousPrecedence = 1000) {
    auto op = token.content();
    auto opType = token.type();
    token.next();

    auto precedence = [&] {
        if (op == "+" || op == "-") {
            return 1;
        }
        else if (opType == Token::Not) {
            return 9;
        }
        throw VBParsingError{token.lastLoc, "Not a unary operator"};
    }();

    auto expression = parseExpression(token, true);

    while (isOperator(token.type())) {
        auto [op, nextPrecedence] = getOperator<LongT>(token.content());
        if (nextPrecedence < precedence) {
            expression = parseBinary(expression, token, precedence);
        }
        else {
            break;
        }
    }

    // TODO: Handle precedence
    if (op == "-") {
        return [expression](Context &context) -> ValueOrRef {
            return expression(context)->negative();
        };
    }
    else if (op == "+") {
        return [expression](Context &context) -> ValueOrRef {
            auto value = expression(context).get();
            if (value.typeName() == Type::String) {
                return Value{value.toFloat()};
            }
            else if (value.isNumber()) {
                return value;
            }
            else {
                throw VBRuntimeError{
                    context.currentLocation(),
                    "Value is not a number, in unary operation"};
            }
        };
    }
    else if (op == "Not") {
        return [expression](Context &context) -> ValueOrRef {
            return {!expression(context)->toBool()};
        };
    }

    throw VBParsingError{
        token.lastLoc,
        "Invalid unary operator, or to many operators in a row " + op};
}

ExpressionT parseBinary(ExpressionT first,
                        TokenPair &token,
                        int previousPrecedence) {

    auto expr = ExpressionT{};
    int currentPrecedence = 1000;

    auto content = token.content();
    auto currentType = token.type();

    token.next();

    auto second = parseExpression(token, true);

    if (currentType == Token::Is) {
        expr = [first, second](Context &context) -> ValueOrRef {
            auto val1 = first(context).get();
            auto val2 = second(context).get();
            return Value{val1.get<ClassT>() == val2.get<ClassT>()};
        };
    }
    else {

        auto opFInt = getOperator<LongT>(content);
        auto opFFloat = getOperator<DoubleT>(content);

        currentPrecedence = opFInt.second;

        for (; token.type() == Token::Operator;) {
            auto nextOp = getOperator<LongT>(token.content());
            auto nextPrecedence = nextOp.second;

            if (nextPrecedence < currentPrecedence) {
                second = parseBinary(second, token, currentPrecedence);
            }
            else {
                break;
            }
        }

        expr =
            [first, second, opFInt, opFFloat](Context &context) -> ValueOrRef {
            auto firstResult = first(context);
            auto secondResult = second(context);
            auto type = firstResult->commonType(secondResult->type());

            switch (type.type) {
            case Type::Integer:
            case Type::Long:
                return Value{opFInt.first(firstResult->toInteger(),
                                          secondResult->toInteger())};
            case Type::Double:
            case Type::Single:
                return Value{opFFloat.first(firstResult->toFloat(),
                                            secondResult->toFloat())};
                break;
            default:
                break;
            }

            throw VBRuntimeError{context.currentLocation(),
                                 "could not convert to numeric value"};
        };
    }

    if (isOperator(token.type())) {
        return parseBinary(expr, token, currentPrecedence);
    }

    return expr;
}

ExpressionT parseParentheses(TokenPair &token) {
    token.next();

    auto expr = parseExpression(token);

    assertEqual(token, Token::ParenthesesEnd, ")");
    token.next();

    return expr;
}

ExpressionT parseExpression(TokenPair &token, bool ignoreBinary) {
    auto keyword = token.type();

    ExpressionT expr;

    auto name = token.content();

    switch (keyword) {
    case Token::StringLiteral:
        token.next();
        expr = [name](Context &) { return ValueOrRef{std::string{name}}; };
        break;
    case Token::NumberLiteral:
        token.next();
        expr = [l = std::stol(name)](Context &) {
            return ValueOrRef{{LongT{l}}};
        };
        break;
    case Token::FloatLiteral:
        token.next();
        expr = [i = std::stod(name)](Context &) {
            return ValueOrRef{{DoubleT{i}}};
        };
        break;

    case Token::ParenthesesBegin:
        expr = parseParentheses(token);
        break;
    case Token::Operator:
    case Token::Not:
        expr = parseUnary(token);
        break;
    case Token::Nothing:
        expr = [](Context &context) {
            return Value::create(Type{
                Type::Class,
                context.globalContext.nothingType.get(),
            });
        };
        token.next();
        break;
    case Token::Rnd:
        expr = [](Context &context) -> ValueOrRef {
            return Value{std::uniform_real_distribution<double>(0, 1)(
                context.globalContext.generator)};
        };
        token.next();
        break;
    case Token::Err:
        expr = [](Context &context) -> ValueOrRef {
            return context.globalContext.err();
        };
        token.next();
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
    case Token::True:
        expr = [](auto &context) { return ValueOrRef{true}; };
        token.next();
        break;
    case Token::False:
        expr = [](auto &context) { return ValueOrRef{false}; };
        token.next();
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

    for (;;) {
        if (isOperator(token.type())) {
            if (!ignoreBinary) {
                expr = parseBinary(expr, token);
            }
            else {
                return expr;
            }
        }
        switch (token.type()) {
        case Token::ParenthesesBegin: {
            token.next();
            auto f = parseFunctionCall(token, expr);
            if (token.type() != Token::ParenthesesEnd) {
                throw VBParsingError{token.lastLoc,
                                     "Expected ')' got " + token.content()};
            }
            token.next();

            expr = f;
            break;
        }
        default:
            return expr;
        }
    }
}

std::vector<ExpressionT> parseList(TokenPair &token) {
    auto list = std::vector<ExpressionT>{};

    if (!token.first || token.type() == Token::ParenthesesEnd) {
        return {};
    }

    if (token.content() == ",") {
        list.emplace_back();
    }
    else {
        list.push_back(parseExpression(token));
    }

    while (token.content() == ",") {
        auto loc = token.first->loc;
        token.next();
        if (token.content() == ",") {
            list.emplace_back();
        }
        else if (!token.first) {
            throw VBParsingError{loc, "expected expression"};
        }
        list.push_back(parseExpression(token));
    }

    return list;
}

Function *lookupFunction(std::string_view name, Context &context) {
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
    Context &context) {
    auto values = FunctionArgumentValues{};
    values.reserve(args.size());

    for (size_t i = 0; i < args.size(); ++i) {
        auto &arg = args.at(i);
        auto &funcArg = functionArgs.at(i);

        if (!arg && funcArg.defaultValue) {
            values.push_back(funcArg.defaultValue(context));
        }
        else if (funcArg.isByRef) {
            values.push_back(arg(context));
        }
        else {
            values.push_back(ValueOrRef{arg(context).get()});
        }
    }

    for (size_t i = values.size(); i < functionArgs.size(); ++i) {
        if (functionArgs.at(i).defaultValue) {
            values.emplace_back(functionArgs.at(i).defaultValue(context));
        }
        else {
            throw VBRuntimeError{context.currentLocation(),
                                 "Wrong number of arguments to function"};
        }
    }

    return values;
}

ExpressionT parseFunctionCall(TokenPair &token,
                              ExpressionT functionExpression) {
    auto argExpressionList = parseList(token);

    return [functionExpression,
            argExpressionList,
            function = static_cast<const Function *>(nullptr),
            loc = token.lastLoc](Context &context) mutable -> ValueOrRef {
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

        return function->call(args, ref.me, context);
    };
}

CommandT parseExitStatement(TokenPair &token) {
    token.next();

    switch (token.type()) {
    case Token::For:
        token.next();
        return [](Context) { return ReturnT::ExitFor; };
    case Token::Sub:
    case Token::Function:
        token.next();
        return [](Context) { return ReturnT::ExitFunction; };
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
        token.next();
        return [](Context) { return ReturnT::ContinueFor; };
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

    do {
        auto name = token.first->content;
        token.next();
        auto type = parseAsStatement(token);
        try {
            token.currentFunctionBody->pushLocalVariable(
                name, type.type, type.shouldCreateNew);
        }
        catch (std::logic_error &e) {
            throw VBParsingError{token.lastLoc,
                                 "Variable " + name + " already exists"};
        }
    } while (token.content() == "," && (token.next(), true));
}

void parseMemberDeclaration(TokenPair &token) {
    auto loc = token.first->loc;
    if (token.type() != Token::Word) {
        token.next();
        parseAssert(token, loc, "Expected variable name");
    }

    auto name = token.content();

    token.next();

    auto type = parseAsStatement(token);

    // TODO: Handle differences between class and modules where modules always
    // have global/static variables and classes as standard have nonstatic
    // variables
    if (token.currentModule->type() == ModuleType::Class) {
        auto classType = token.currentModule->classType.get();

        classType->addAddVariable(name, type.type, type.shouldCreateNew);
    }
    else {
        token.currentModule->addStaticVariable(
            name, type.type, type.shouldCreateNew);
    }
}

void parseMemberDeclarationList(TokenPair &token) {
    parseMemberDeclaration(token);

    // TODO: Loop for all declarations on line
}

FunctionBody::CommandT parseAssignment(TokenPair &token,
                                       IdentifierFuncT target,
                                       bool isSetStatement) {
    auto loc = token.first->loc;

    if (token.content() != "=") {
        throw VBParsingError{loc, "expected '=' got " + token.content()};
    }
    token.next();

    if (!token) {
        throw VBParsingError{loc, "Expected expression to assign to"};
    }

    auto expr = parseExpression(token);

    return [target, expr, isSetStatement](Context &context) {
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
        return [expr](Context &context) {
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
    case Token::End:
        token.next();
        return [](Context &context) {
            context.globalContext.end();
            return ReturnT::Return;
        };
    case Token::Set: {
        token.next();
        return parseAssignment(token, parseIdentifier(token), true);
    }
    case Token::Randomize:
        token.next();
        return [](Context &context) {
            context.globalContext.generator.seed(std::random_device{}());
            return ReturnT::Standard;
        };
    case Token::Period:
    case Token::Me:
    case Token::MsgBox:
    case Token::Word: {
        {
            auto identifier = parseIdentifier(token);

            if (token.content() == "=") {
                return parseAssignment(token, identifier, false);
            }
            else {
                auto f = parseFunctionCall(token, identifier);
                return [f](Context &context) {
                    f(context);
                    return ReturnT::Standard;
                };
            }
        }
    }
    case Token::Empty:
        throw VBParsingError{token.lastLoc, "unexpected end of line"};
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

        auto type = parseAsStatement(token);

        structType.addAddVariable(memberName, type.type, type.shouldCreateNew);
    }

    nextLine();
}

FunctionBody::CommandT parseSelectCase(Line **line,
                                       TokenPair &token,
                                       NextLineT nextLine) {
    token.next();
    if (token.type() != Token::Case) {
        VBParsingError{token.lastLoc, "expected 'Case' got " + token.content()};
    }
    token.next();
    auto expression = parseExpression(token);

    *line = nextLine();

    auto cases = std::vector<std::pair<ExpressionT, CommandT>>{};

    token.next();

    auto caseElse = CommandT{};

    for (;;) {
        if (token.type() != Token::Case) {
            throw VBParsingError{
                token.lastLoc, "Expected token 'Case'  got " + token.content()};
        }

        token.next();

        if (token.type() == Token::Else) {
            caseElse = parseBlock(
                line,
                token,
                nextLine,
                [](auto token) {
                    return token == Token::Case || token == Token::End;
                },
                false);
        }
        else {
            //! Just hope that it happends to be constant
            cases.push_back({parseExpression(token),
                             parseBlock(
                                 line,
                                 token,
                                 nextLine,
                                 [](auto token) {
                                     return token == Token::Case ||
                                            token == Token::End;
                                 },
                                 false)});
        }

        if (token.type() == Token::End) {
            break;
        }
    }

    *line = nextLine();

    return [expression, cases, caseElse](Context &context) {
        auto value = expression(context).get();

        for (auto &c : cases) {
            if (c.first(context).get() == value) {
                if (auto ret = c.second(context); ret != ReturnT::Standard) {
                    return ret;
                }
            }
        }

        if (caseElse) {
            return caseElse(context);
        }

        return ReturnT::Standard;
    };
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
            return [](Context &context) -> Value { return true; };
        }
        else {
            return parseExpression(token);
        }
    }();

    if (!isElse) {
        if (token.type() == Token::Then) {
            token.next();
            if (token) {
                auto block = parseCommand(token);
                if (token.type() == Token::Else) {
                    token.next();
                    auto elseBlock = parseCommand(token);
                    *line = nextLine();
                    return [condition, block, elseBlock](
                               Context &context) -> ReturnT {
                        if (condition(context)->toBool()) {
                            return block(context);
                        }
                        else {
                            return elseBlock(context);
                        }
                    };
                }
                *line = nextLine();
                return [condition, block](Context &context) -> ReturnT {
                    if (condition(context)->toBool()) {
                        return block(context);
                    }
                    return ReturnT::Standard;
                };
            }
        }
    }

    // TODO: Handle inline if statements
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

    return [condition, codeBlock, elseStatement](Context &context) {
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
        auto type = std::optional<TypeDescription>{};

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

        return [codeBlock](Context &context) {
            std::cerr << "for each not implemented yet" << std::endl;
            return ReturnT::Standard;
        };
    }

    auto name = token.content();

    token.next();

    auto isScopeLocalVariable = false;

    if (isTypeDeclaration(token)) {
        isScopeLocalVariable = true;
        auto type = parseAsStatement(token);

        token.currentFunctionBody->pushLocalVariable(
            name, type.type, type.shouldCreateNew);
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

    auto step = ExpressionT{[](Context &) -> ValueOrRef { return Value{1}; }};

    if (token.type() == Token::Step) {
        token.next();
        step = parseExpression(token);
    }

    auto code = parseBlock(
        line, token, nextLine, [](auto t) { return t == Token::Next; });

    if (isScopeLocalVariable) {
        token.currentFunctionBody->forgetLocalVariableName(name);
    }

    return [start, code, stop, step, variableIdentification](Context &context) {
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
    return [identifier](Context &context) {
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
    auto withIdentifier = [](Context &context) -> ValueOrRef {
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
            if (!token.second) {
                // End statement
                block.addCommand([](auto &context) -> ReturnT {
                    context.globalContext.end();
                    return ReturnT::Standard;
                });
                *line = nextLine();
            }
            else if (token.second->type() == Token::With) {
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
                [block = std::move(block)](Context &context) {
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
                assertEmpty(token);
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
    auto function =
        std::make_unique<Function>(name, std::move(arguments), isStatic);
    body->function(function.get());

    body->pushCommand(parseBlock(line,
                                 token,
                                 nextLine,
                                 [](Token::Keyword t) {
                                     return t == Token::End; //
                                 }),
                      token.lastLoc.line);

    Function::FuncT f = [body](const FunctionArgumentValues &args,
                               Value me,
                               Context &context) -> Value {
        return body->call(args, me, context);
    };

    function->body(f);

    token.endFunction();

    return function;
}

Module &parseGlobal(Line *line,
                    NextTokenT nextToken,
                    NextLineT nextLine,
                    std::filesystem::path path,
                    GlobalContext &global,
                    Module &module) {

    Scope currentScope;

    for (; line;) {
        currentScope = Private;
        TokenPair token{};
        token.f = nextToken;
        token.currentModule = &module;
        token.next();
        token.modules = &global.modules;

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
        case Token::Function:
            module.addFunction(parseFunction(&line, Private, token, nextLine));
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
        case Token::Type:
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
        case Token::Function:
            module.addFunction(
                parseFunction(&line, currentScope, token, nextLine));
            break;

        case Token::Word:
            parseMemberDeclaration(token);
            line = nextLine();
            break;

        case Token::Structure:
        case Token::Type:
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

Module &prescanModule(std::filesystem::path path, GlobalContext &global) {
    auto module = std::make_unique<Module>(path);

    if (path.extension() == ".cls" || path.extension() == ".frm") {
        module->classType = std::make_unique<ClassType>();
        module->classType->module = module.get();
        module->classType->name = path.stem();

        if (path.extension() == ".frm") {
            module->addStaticVariable(
                module->classType->name,
                Type{Type::Class, module->classType.get()},
                true);
        }
    }

    // TODO: Parse struct names, and global functions

    global.modules.push_back(std::move(module));
    return *global.modules.back();
}

Module &parse(std::istream &stream,
              std::filesystem::path path,
              GlobalContext &global) {

    auto moduleName = path.stem();
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

    auto &module = [&]() -> Module & {
        for (auto &module : global.modules) {
            if (module->name() == moduleName) {
                return *module;
            }
        }
        return prescanModule(path, global);
    }();

    return parseGlobal(line, nextToken, nextLine, path, global, module);
}

Module &loadModule(std::filesystem::path path, GlobalContext &global) {
    auto file = std::ifstream{path};

    if (!file) {
        throw VBParsingError{
            Location{
                .line = 0,
                .path = std::make_shared<std::filesystem::path>(path),
            },
            "Could not load file " + path.string()};
    }
    return parse(file, path, global);
}
