#include "Parser.h"
#include "Lexer.h"
#include "TokenType.h"
#include <gtest/gtest.h>
#include <iostream>

namespace {
TEST(MatchType, DefaultConstructor) {
    using namespace minidb;
    Parser parser("select a from b;");
    std::cout << "parser.selectStatement()" << std::endl;
    if (parser.selectStatement()) {
        EXPECT_EQ(false, false);
    } else {
        EXPECT_EQ(true, false);
    }
    // EXPECT_EQ(parser.chain("123", []() { return true; }), false);
}
} // namespace