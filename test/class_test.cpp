#include "classinstance.h"
#include "classtype.h"
#include "function.h"
#include "mls-unit-test/unittest.h"

TEST_SUIT_BEGIN

TEST_CASE("instantiate class") {
    auto c = ClassType{};
    c.addAddVariable("x", Type{Type::Integer});

    auto i = ClassInstance::create(&c);
    i->set("x", IntegerT{130});

    EXPECT_EQ(std::get<IntegerT>(i->get("x").value), 130);
}

TEST_SUIT_END
