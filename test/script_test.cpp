
#include "mls-unit-test/unittest.h"
#include <filesystem>
#include <sstream>

TEST_SUIT_BEGIN;

for (auto &it : std::filesystem::recursive_directory_iterator{"scripts"}) {
    unittest::testMap[it.path()] = [path = it.path()]() -> void {
        std::cout << path << std::endl;
    };
}

TEST_SUIT_END
