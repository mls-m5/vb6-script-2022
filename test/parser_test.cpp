#include "mls-unit-test/unittest.h"
#include "parser.h"
#include <sstream>

TEST_SUIT_BEGIN

TEST_CASE("basic parsing") {
    auto ss = std::istringstream{R"_(

Private Sub Main()

End Sub
)_"};

    auto module = parse(ss, "");
}

TEST_SUIT_END
