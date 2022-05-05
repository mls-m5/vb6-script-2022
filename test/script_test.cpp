
#include "Location.h"
#include "functionbody.h"
#include "mls-unit-test/unittest.h"
#include "parser.h"
#include "testcontext.h"
#include <filesystem>
#include <fstream>

// Add possibility to load class files associated with module files
std::vector<std::filesystem::path> parseImports(std::filesystem::path path) {
    auto list = std::vector<std::filesystem::path>{};
    auto file = std::ifstream{path};

    auto str = std::string_view{"' TestImport "};
    for (std::string line; std::getline(file, line);) {
        if (line.rfind(str) == 0) {
            auto name = line.substr(str.size());
            list.push_back("scripts/" + name + ".cls");
        }
        else {
            break;
        }
    }

    return list;
}

TEST_SUIT_BEGIN;

auto innerAssert = Function::FuncT{
    [&](const FunctionArgumentValues &args, Value, Context &context) {
        if (args.size() != 1) {
            throw std::runtime_error{
                "assert called with wrong number of arguments"};
        }

        auto &value = args.at(0).get();
        auto index = value.value.index();

        std::cout << value.toString() << std::endl;
        auto arg = value.toInteger();
        if (!arg) {
            throw std::runtime_error{context.module->path.string() + ":" +
                                     std::to_string(context.line) +
                                     ": assertion failed "};
        }
        return Value{};
    }};

for (auto &it : std::filesystem::recursive_directory_iterator{"scripts"}) {
    if (it.path().extension() != ".bas") {
        continue;
    }

    unittest::testMap[it.path().string()] = [path = it.path(),
                                             innerAssert]() -> void {
        std::cout << path << std::endl;

        auto context = TestContext{nullptr};

        auto imports = parseImports(path);

        for (auto &import : imports) {
            context.global.modules.push_back(
                loadModule(import, context.global.modules));
        }

        auto module = loadModule(path, context.global.modules);

        auto testModule = std::make_shared<Module>();

        testModule
            ->addFunction(std::make_unique<Function>(
                "Assert",
                FunctionArguments{{
                    FunctionArgument{Type{Type::Integer}, "x"},
                }},
                true))
            .body(innerAssert);

        context.global.modules.push_back(testModule);

        context.local.module = module.get();

        module->function("Main")->call({}, {}, context.local);
    };
}

TEST_SUIT_END
