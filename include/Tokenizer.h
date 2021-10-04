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
    int                                               pos;
    std::string                                       sql;
    static std::unordered_map<std::string, TokenType> symbolTokenType;
    static std::unordered_map<std::string, TokenType> keywordTokenType;

    void  toLowerCase(std::string &str);
    Token getIdToken();
    Token getSymbolToken();
    Token getLiteralToken();
    Token getNumberToken();
};
} // namespace minidb