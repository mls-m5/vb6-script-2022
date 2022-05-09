#include "array.h"
#include "classinstance.h"
#include "mls-unit-test/unittest.h"
#include "value.h"

TEST_SUIT_BEGIN

TEST_CASE("Basic array") {
    auto array = ArrayT{Type{Type::Integer}, 10};

    array.at(4);

    ASSERT_EQ(array.at(4).toInteger(), 0);

    array.at(4) = 20;
    ASSERT_EQ(array.at(4).toInteger(), 20);
}

TEST_CASE("Redim") {
    auto array = ArrayT{Type{Type::Long}};

    array.redim(10);

    array.at(4) = 10l;

    ASSERT_EQ(array.at(4).typeName(), Type::Long);
    ASSERT_EQ(array.at(4).toInteger(), 10);
}

TEST_SUIT_END
