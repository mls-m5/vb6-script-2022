
#include "Location.h"
#include "functionbody.h"
#include "mls-unit-test/unittest.h"
#include "parser.h"
#include "testcontext.h"
#include <filesystem>

TEST_SUIT_BEGIN;

auto innerAssert = Function::FuncT{
    [&](const FunctionArgumentValues &args, LocalContext &context) {
        if (args.size() != 1) {
            throw std::runtime_error{
                "assert called with wrong number of arguments"};
        }

        auto &value = args.at(0).get();
        auto index = value.value.index();

        std::cout << value.toString() << std::endl;
        auto arg = value.get<IntegerT>();
        if (!arg) {
            throw std::runtime_error{context.module->path.string() + ":" +
                                     std::to_string(context.line) +
                                     ": assertion failed "};
        }
        return Value{};
    }};

for (auto &it : std::filesystem::recursive_directory_iterator{"scripts"}) {
    if (it.path().extension() != ".mod") {
        continue;
    }

    unittest::testMap[it.path().string()] = [path = it.path(),
                                             innerAssert]() -> void {
        std::cout << path << std::endl;

        auto module = loadModule(path);

        module->addFunction(std::make_unique<Function>(
            "Assert",
            FunctionArguments{{
                FunctionArgument{Type{Type::Integer}, "x"},
                FunctionArgument{Type{Type::Integer}, "y"},
            }},
            innerAssert));

        auto context = TestContext{module.get()};

        module->function("Main")->call({}, context.local);
    };
}

TEST_SUIT_END
