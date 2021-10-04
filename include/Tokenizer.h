#pragma once
#include "Token.h"
#include "TokenType.h"
#include <cctype>
#include <string>
#include <unordered_map>

namespace minidb {

class Tokenizer {
public:
    Tokenizer(const std::string &_sql);
    Token getNextToken();

protected:
    std::string                            sql;
    int                                    pos;
    static std::unordered_map<char, Token>        singleCharacterToken;
    static std::unordered_map<std::string, Token> doubleCharacterToken;

    Token getSymbolToken();
    Token getLiteralToken();
    Token getNumberToken();
};
} // namespace minidb