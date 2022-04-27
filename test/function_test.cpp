#include "function.h"
#include "mls-unit-test/unittest.h"

TEST_SUIT_BEGIN

TEST_CASE("instantiate class") {
    auto f = Function{"hello", {}};

    f.call({});
}

TEST_SUIT_END
