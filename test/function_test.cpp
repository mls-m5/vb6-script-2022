#include "function.h"
#include "functionbody.h"
#include "mls-unit-test/unittest.h"

TEST_SUIT_BEGIN

TEST_CASE("basic function call") {
    auto a = -1, b = -1, c = -1;

    auto lambda = [&](const FunctionArgumentValues &args,
                      LocalContext &context) -> Value {
        a = args.at(0).get().get<IntegerT>();
        b = args.at(1).get().get<IntegerT>();
        c = args.at(2).get().get<IntegerT>();
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

TEST_CASE("basic function body") {
    auto body = FunctionBody{};

    body.pushCommand([](LocalContext &context) {
        context.returnValue = IntegerT{20}; //
    });

    auto globalContext = GlobalContext{};
    auto dummyContext = LocalContext{globalContext};

    auto ret = body.call({}, dummyContext);

    ASSERT_EQ(ret.typeName(), Type::Integer);
    ASSERT_EQ(ret.get<IntegerT>(), 20);
}

TEST_CASE("Value or ref") {
    // Problematic expressions
    auto value = Value{IntegerT{10}};

    std::cout << value.toString() << std::endl;

    auto vor = ValueOrRef{value};

    std::cout << vor.get().toString() << std::endl;
}

TEST_SUIT_END
