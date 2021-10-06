#pragma once
#include "Token.h"
#include "TokenType.h"
#include <cctype>
#include <string>
#include <unordered_map>
#include <vector>
#include <map>

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
    static std::multimap<char, std::string> keywordFirstLetter;

        void
          toLowerCase(std::string &str);
    Token getSymbolToken();
    Token getLiteralToken();
    Token getNumberToken();
};
} // namespace minidb