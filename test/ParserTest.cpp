#include "Lexer.h"
#include "TokenType.h"
#include "Parser.h"
#include <gtest/gtest.h>

namespace {
TEST(MatchType, DefaultConstructor) {
    using namespace minidb;
    Parser parser;
    Parser::MatchType matchType("123");
    parser.chain({"123"});
    EXPECT_EQ(matchType.isString(), true);
    EXPECT_EQ(matchType.isFunc(), false);
}
} // namespace