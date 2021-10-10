#include "Lexer.h"
#include "TokenType.h"
#include "Parser.h"
#include <gtest/gtest.h>

namespace {
TEST(MatchType, DefaultConstructor) {
    using namespace minidb;
    Parser parser;
    EXPECT_EQ(parser.chain("123"), false);
    EXPECT_EQ(parser.chain("123", []() { return true; }), false);
}
} // namespace