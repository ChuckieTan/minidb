#include "Tokenizer.h"
#include "TokenType.h"
#include "Token.h"
#include <gtest/gtest.h>


namespace {
    TEST(Tokenizer, DefaultConstructor) {
        using namespace minidb;
        Tokenizer tokenizer(std::string("select * from student"));
        EXPECT_EQ(tokenizer.getNextToken().tokenType, TokenType::SELECT);
        EXPECT_EQ(tokenizer.getNextToken().tokenType, TokenType::STAR);
        EXPECT_EQ(tokenizer.getNextToken().tokenType, TokenType::FROM);
        EXPECT_EQ(tokenizer.getNextToken(),
                  Token(TokenType::IDENTIFIER, "student"));
    }
}