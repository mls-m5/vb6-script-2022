#include "function.h"
#include "mls-unit-test/unittest.h"

TEST_SUIT_BEGIN

TEST_CASE("instantiate class") {
    auto a = -1, b = -1, c = -1;

    auto lambda = [&](const FunctionArgumentValues &args,
                      LocalContext &context) -> Value {
        a = std::get<IntegerT>(args.at(0).get().value);
        b = std::get<IntegerT>(args.at(1).get().value);
        c = std::get<IntegerT>(args.at(2).get().value);
        return 120;
    };

    auto global = GlobalContext{};
    auto context = LocalContext{global};

    auto f = Function{"hello",
                      FunctionArguments{
                          {
                              FunctionArgument{Type{Type::Integer}, "x"},
                              FunctionArgument{Type{Type::Integer}, "y"},
                          },
                      },
                      lambda};

    auto args = FunctionArgumentValues{};
    args.push_back({10});
    args.push_back({20});
    auto value = Value{30};
    args.push_back({&value});

    ASSERT_EQ(std::get<IntegerT>(f.call(args, context).value), 120);

    ASSERT_EQ(a, 10);
    ASSERT_EQ(b, 20);
    ASSERT_EQ(c, 30);
}

TEST_SUIT_END
