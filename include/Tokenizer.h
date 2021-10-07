#pragma once
#include "Token.h"
#include "TokenType.h"
#include <cctype>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace minidb {

class Tokenizer {
public:
    struct SavePoint {
        int pos;
    };
    Tokenizer(const std::string &_sql);
    Token getNextToken();

    SavePoint mark();
    void reset(int _pos);
    void reset(SavePoint savePoint);

protected:
    int                                               pos;
    std::string                                       sql;
    static std::unordered_map<std::string, TokenType> symbolTokenType;
    static std::unordered_map<std::string, TokenType> keywordTokenType;
    static std::multimap<char, std::string>           keywordFirstLetter;

    void  toLowerCase(std::string &str);
    Token getSymbolToken();
    Token getLiteralToken();
    Token getNumberToken();
};
} // namespace minidb