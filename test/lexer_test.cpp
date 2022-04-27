#include "lexer.h"
#include "mls-unit-test/unittest.h"
#include <sstream>

TEST_SUIT_BEGIN

TEST_CASE("basic tokens") {
    auto ss = std::istringstream{"(*=\n2-4,"};

    const auto f = CodeFile{ss, "test"};

    EXPECT_EQ(f.lines.size(), 2);
    EXPECT_EQ(f.lines.front().size(), 3);
    EXPECT_EQ(f.lines.back().size(), 4);
}

TEST_CASE("more complicated") {
    auto ss = std::istringstream{"private sub main()"};

    const auto f = CodeFile{ss, "test"};

    EXPECT_EQ(f.lines.size(), 1);

    auto &line = f.lines.front();
    EXPECT_EQ(line.size(), 5);
}

TEST_CASE("broken line") {
    auto ss = std::istringstream{"broken _ \n line"};

    const auto f = CodeFile{ss, "test"};
    EXPECT_EQ(f.lines.size(), 1);
    EXPECT_EQ(f.lines.back().size(), 2);
}

TEST_SUIT_END
