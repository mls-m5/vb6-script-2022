#include "code/project.h"
#include "mls-unit-test/unittest.h"
#include <sstream>

TEST_SUIT_BEGIN

TEST_CASE("load old project") {
    auto project = Project{"Test5.1/Project1.vbp"};
}

TEST_SUIT_END
