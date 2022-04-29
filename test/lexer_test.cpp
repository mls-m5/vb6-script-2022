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

TEST_CASE("keywords") {
    auto ss = std::istringstream{"Private Sub function"};

    const auto f = CodeFile{ss, "test"};

    EXPECT_EQ(f.lines.size(), 1);

    auto &line = f.lines.front();
    EXPECT_EQ(line.size(), 3);
    EXPECT_EQ(line.at(0).type(), Token::Private);
    EXPECT_EQ(line.at(1).type(), Token::Sub);
    EXPECT_EQ(line.at(2).type(), Token::Function);
}

TEST_CASE("broken line") {
    auto ss = std::istringstream{"broken _ \n line"};

    const auto f = CodeFile{ss, "test"};
    EXPECT_EQ(f.lines.size(), 1);
    EXPECT_EQ(f.lines.back().size(), 2);
}

TEST_CASE("quotation") {
    auto ss = std::istringstream{"\"hello\""};

    const auto f = CodeFile{ss, "test"};
    EXPECT_EQ(f.lines.size(), 1);
    EXPECT_EQ(f.lines.back().size(), 1);
    EXPECT_EQ(f.lines.back().back().content, "\"hello\"");
}

TEST_CASE("comment") {
    auto ss = std::istringstream{"hello ' there you"};

    const auto f = CodeFile{ss, "test"};
    EXPECT_EQ(f.lines.size(), 1);
    EXPECT_EQ(f.lines.back().size(), 1);
    EXPECT_EQ(f.lines.back().back().content, "hello");
}

TEST_CASE("remove blank lines") {
    auto ss = std::istringstream{"hello \n\n there you"};

    const auto f = CodeFile{ss, "test"};
    EXPECT_EQ(f.lines.size(), 2);
}

TEST_CASE("words with numbers in them") {
    auto ss = std::stringstream{"hello2you2"};

    const auto f = CodeFile{ss, ""};
    EXPECT_EQ(f.lines.size(), 1);
    std::cout << f.lines.front().front().content << std::endl;

    EXPECT_EQ(f.lines.front().size(), 1);
}

TEST_CASE("floating point numbers") {
    auto ss = std::stringstream{"2.8"};

    const auto f = CodeFile{ss, ""};
    EXPECT_EQ(f.lines.size(), 1);
    std::cout << f.lines.front().front().content << std::endl;

    EXPECT_EQ(f.lines.front().size(), 1);
}

TEST_SUIT_END
