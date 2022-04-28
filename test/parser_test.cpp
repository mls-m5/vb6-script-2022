#include "mls-unit-test/unittest.h"
#include "parser.h"
#include "testcontext.h"
#include <sstream>

TEST_SUIT_BEGIN

TEST_CASE("basic parsing") {
    auto ss = std::istringstream{R"_(

Private Sub Main()
    Print "hello there"
End Sub
)_"};

    auto module = parse(ss, "");

    auto context = TestContext{};

    auto &f = module.function("Main");

    f.call(context.args, context.local);
}

TEST_CASE("ignore option statements") {
    auto ss = std::istringstream{R"_(
Option Explicit

Private Sub Main()
    Print "hello there"
End Sub
)_"};

    auto module = parse(ss, "");
}

TEST_CASE("Call global function") {
    auto ss = std::istringstream{R"_(
Option Explicit

Private Sub Main()
    TestSet 10, 20
End Sub
)_"};

    auto module = parse(ss, "");

    auto context = TestContext{};

    auto inner = Function::FuncT{[](const FunctionArgumentValues &args,
                                    LocalContext &) { return Value{}; }};

    module.addFunction(std::make_unique<Function>(
        "TestSet",
        FunctionArguments{{
            FunctionArgument{Type{Type::Integer}, "x"},
            FunctionArgument{Type{Type::Integer}, "y"},
        }},
        inner));

    auto &f = module.function("Main");

    f.call(context.args, context.local);

    ASSERT_EQ(context.global.get("test").get<IntegerT>(), 10);
}
TEST_SUIT_END
