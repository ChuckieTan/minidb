#include "Parser.h"
#include "Lexer.h"
#include "TokenType.h"
#include <gtest/gtest.h>
#include <iostream>

namespace {
TEST(MatchType, SelectStatement) {
    using namespace minidb;
    Parser parser("select a from b;");
    std::cout << "parser.selectStatement()" << std::endl;
    EXPECT_EQ(parser.selectStatement(), true);
    parser = Parser("select sum(score) from student;");
    EXPECT_EQ(parser.selectStatement(), true);
}
} // namespace