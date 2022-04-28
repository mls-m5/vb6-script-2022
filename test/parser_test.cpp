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

TEST_SUIT_END
