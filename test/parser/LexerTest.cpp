#include "Lexer.h"
#include "TokenType.h"
#include "Token.h"
#include <gtest/gtest.h>


namespace {
    TEST(Lexer, DefaultConstructor) {
        using namespace minidb::parser;
        Lexer lexer(std::string("select * from student"));
        EXPECT_EQ(lexer.getNextToken().tokenType, TokenType::SELECT);
        EXPECT_EQ(lexer.getNextToken().tokenType, TokenType::STAR);
        EXPECT_EQ(lexer.getNextToken().tokenType, TokenType::FROM);
        EXPECT_EQ(lexer.getNextToken(),
                  Token(TokenType::IDENTIFIER, "student"));
    }
}