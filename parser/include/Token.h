#pragma once
#include "TokenType.h"
#include <any>
#include <string>

namespace minidb::parser {
struct Token {
    TokenType   tokenType;
    std::string val;
    Token(TokenType          _tokenType = TokenType::ILLEGAL,
          const std::string &_val       = "")
        : tokenType(_tokenType)
        , val(_val) {
    }
    bool operator==(const Token &token) const {
        return (tokenType == token.tokenType) && (val == token.val);
    }
};
} // namespace minidb