#pragma once
#include "TokenType.h"
#include <string>

namespace minidb {
struct Token {
    TokenType   tokenType;
    std::string val;
    Token(TokenType          _tokenType = TokenType::ILLEGAL,
          const std::string &_val       = "")
        : tokenType(_tokenType)
        , val(_val) {
    }
};
} // namespace minidb