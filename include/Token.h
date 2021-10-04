#pragma once
#include "TokenType.h"
#include <any>
#include <string>

namespace minidb {
struct Token {
    TokenType tokenType;
    std::any  val;
    Token(TokenType       _tokenType = TokenType::ILLEGAL,
          const std::any &_val       = std::any())
        : tokenType(_tokenType)
        , val(_val) {
    }
};
} // namespace minidb