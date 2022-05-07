#include "classinstance.h"
#include "classtype.h"
#include "function.h"
#include "mls-unit-test/unittest.h"
#include "type.h"

TEST_SUIT_BEGIN

TEST_CASE("create struct") {
    auto classType = ClassType{};

    classType.addAddVariable("x", {Type::Integer}, false);

    auto s = StructT{&classType};
    auto value = Value{s};

    value.get<StructT>()->set(0, 20);

    auto value2 = value;

    ASSERT_EQ(value2.get<StructT>()->get(0).get<IntegerT>(), 20);
}

TEST_CASE("send as argument") {
    auto classType = ClassType{};

    classType.addAddVariable("x", {Type::Integer}, false);

    auto s = StructT{&classType};
    auto value = Value{s};

    value.get<StructT>()->set(0, 20);

    auto args = FunctionArgumentValues{{value}};
    auto value2 = args.at(0).get();

    ASSERT_EQ(value2.get<StructT>()->get(0).get<IntegerT>(), 20);
}

TEST_SUIT_END
