
#include "mls-unit-test/unittest.h"
#include "parser.h"
#include "testcontext.h"
#include <filesystem>

TEST_SUIT_BEGIN;

auto inner =
    Function::FuncT{[&](const FunctionArgumentValues &args, LocalContext &) {
        if (args.size() != 1) {
            throw std::runtime_error{
                "assert called with wrong number of arguments"};
        }

        auto &value = args.at(0).get();
        auto index = value.value.index();

        //        std::cout << value.toString() << std::endl;
        auto arg = value.get<IntegerT>();
        if (!arg) {
            throw std::runtime_error{"assertion failed"};
        }
        return Value{};
    }};

for (auto &it : std::filesystem::recursive_directory_iterator{"scripts"}) {
    unittest::testMap[it.path().string()] = [path = it.path(),
                                             inner]() -> void {
        std::cout << path << std::endl;

        auto module = loadModule(path);

        module.addFunction(std::make_unique<Function>(
            "Assert",
            FunctionArguments{{
                FunctionArgument{Type{Type::Integer}, "x"},
                FunctionArgument{Type{Type::Integer}, "y"},
            }},
            inner));

        auto context = TestContext{&module};

        module.function("Main")->call({}, context.local);
    };
}

TEST_SUIT_END
