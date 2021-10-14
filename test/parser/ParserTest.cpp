#include "Parser.h"
#include "Lexer.h"
#include "TokenType.h"
#include <gtest/gtest.h>
#include <iostream>

namespace {
TEST(MatchType, SelectStatement) {
    using namespace minidb::parser;

    // simple select statement
    Parser parser("select a from b;");
    EXPECT_EQ(parser.selectStatement(), true);

    // select statement with function
    parser = Parser("select sum(score) from student;");
    EXPECT_EQ(parser.selectStatement(), true);

    // select statement with multiple select list
    parser = Parser("select sum(a), a, abs(b) from student;");
    EXPECT_EQ(parser.selectStatement(), true);

    // select statement with star
    parser = Parser("select count(*) from student;");
    EXPECT_EQ(parser.selectStatement(), true);

    // select statement with nested function
    parser = Parser("select sum(sum(a)) from b;");
    EXPECT_EQ(parser.selectStatement(), true);
}
} // namespace