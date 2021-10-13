#pragma once
#include "Token.h"
#include "TokenType.h"
#include <cctype>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace minidb {

class Lexer {
public:
    struct SavePoint {
        int tokenPos;
    };
    Lexer(std::string &&sql);
    Lexer(const std::string &_sql);
    Token getNextToken();
    Token getCurrentToken();
    Token scanNextToken();
    void  scanTokens();

    SavePoint mark();
    void      reset(int _tokenPos);
    void      reset(SavePoint savePoint);

protected:
    int                                               sqlPos;
    std::string                                       sql;
    int                                               tokenPos;
    std::vector<Token>                                tokenSequence;
    static std::unordered_map<std::string, TokenType> symbolTokenType;
    static std::unordered_map<std::string, TokenType> keywordTokenType;
    static std::multimap<char, std::string>           keywordFirstLetter;

    void  toLowerCase(std::string &str);
    Token getSymbolToken();
    Token getLiteralToken();
    Token getNumberToken();
};
} // namespace minidb