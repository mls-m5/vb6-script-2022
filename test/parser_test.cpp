#include "classtype.h"
#include "function.h"
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

    auto context = TestContext{};

    auto &module = parse(ss, "", context.global);

    auto f = module.function("Main");

    EXPECT(f);

    f->call(context.args, {}, context.local);
}

TEST_CASE("ignore option statements") {
    auto ss = std::istringstream{R"_(
Option Explicit

Private Sub Main()
    Print "hello there"
End Sub
)_"};

    auto context = TestContext{};

    parse(ss, "", context.global);
}

TEST_CASE("Call global function") {
    auto ss = std::istringstream{R"_(
Option Explicit

Private Sub Main()
    TestSet 10, 20
End Sub
)_"};

    auto context = TestContext{};
    auto testModule = std::make_shared<Module>();
    context.global.modules.push_back(testModule);

    bool isCalled = false;
    int numArgs = 0;
    int arg1Res = -1;
    int arg2Res = -1;

    auto inner = Function::FuncT{
        [&](const FunctionArgumentValues &args, Value, Context &context) {
            context.module = testModule.get();
            isCalled = true;
            numArgs = args.size();
            if (numArgs != 2) {
                return Value{};
            }
            arg1Res = args.at(0)->toInteger();
            arg2Res = args.at(1)->toInteger();
            return Value{};
        }};

    testModule
        ->addFunction(std::make_unique<Function>(
            "TestSet",
            FunctionArguments{{
                FunctionArgument{Type{Type::Integer}, "x"},
                FunctionArgument{Type{Type::Integer}, "y"},
            }},
            true))
        .body(inner);

    auto &module = parse(ss, "", context.global);
    context.local.module = &module;

    auto f = module.function("Main");

    EXPECT(f);

    f->call(context.args, {}, context.local);

    ASSERT_EQ(isCalled, true);
    ASSERT_EQ(numArgs, 2);
    ASSERT_EQ(arg1Res, 10);
    ASSERT_EQ(arg2Res, 20);
}
TEST_SUIT_END
