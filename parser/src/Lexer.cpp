#include "Lexer.h"
#include "Token.h"
#include "TokenType.h"
#include "spdlog/fmt/bundled/core.h"
#include <algorithm>
#include <cctype>
#include <map>
#include <string>
#include <system_error>
#include <unordered_map>
#include <vector>

namespace minidb::parser {

std::unordered_map<std::string, TokenType> Lexer::symbolTokenType = {
    { ",", TokenType::COMMA },      { "*", TokenType::STAR },
    { "(", TokenType::LBRACKET },   { ")", TokenType::RBRACKET },
    { "+", TokenType::PLUS },       { "-", TokenType::MINUS },
    { ";", TokenType::SEMICOLON },  { "==", TokenType::EQUAL },
    { "!=", TokenType::NOT_EQUAL }, { "<=", TokenType::LESS_OR_EQUAL },
    { "<>", TokenType::NOT_EQUAL }, { ">=", TokenType::GREATER_OR_EQUAL },
    { "=", TokenType::ASSIGN },     { ".", TokenType::DOT },
    { "/", TokenType::DIV },        { "%", TokenType::MOD }

};

Lexer::Lexer(std::string &&_sql)
    : sqlPos(0)
    , tokenPos(0)
    , sql(_sql) {
    scanTokens();
}

Lexer::Lexer(const std::string &_sql)
    : sqlPos(0)
    , tokenPos(0)
    , sql(_sql) {
    scanTokens();
}

Lexer::SavePoint Lexer::mark() {
    SavePoint savePoint;
    savePoint.tokenPos = tokenPos;
    return savePoint;
}

void Lexer::reset(int _tokenPos) { tokenPos = _tokenPos; }

void Lexer::reset(SavePoint savePoint) { tokenPos = savePoint.tokenPos; }

Token Lexer::getSymbolToken() {
    Token token;
    if (auto ch = sql.substr(sqlPos, 2); symbolTokenType.count(ch)) {
        token = Token(symbolTokenType[ ch ], ch);
        sqlPos += 2;
    } else if (auto ch = sql.substr(sqlPos, 1); symbolTokenType.count(ch)) {
        token = Token(symbolTokenType[ ch ], ch);
        sqlPos += 1;
    } else {
        if (sql[ sqlPos ] == '\'') {
            sqlPos++;
            int len = 0;
            while (sql[ sqlPos + len ] != '\'') {
                len++;
            }
            token = Token(TokenType::STRING, sql.substr(sqlPos, len));
            sqlPos += len + 1;
        } else {
            token = Token(TokenType::ILLEGAL, sql.substr(sqlPos, 1));
            sqlPos += 1;
        }
    }
    return token;
}

void Lexer::toLowerCase(std::string &str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

std::unordered_map<std::string, TokenType> Lexer::keywordTokenType = {
    { "create", TokenType::CREATE },   { "table", TokenType::TABLE },
    { "insert", TokenType::INSERT },   { "into", TokenType::INTO },
    { "values", TokenType::VALUES },   { "delete", TokenType::DELETE },
    { "drop", TokenType::DROP },       { "select", TokenType::SELECT },
    { "from", TokenType::FROM },       { "where", TokenType::WHERE },
    { "and", TokenType::AND },         { "or", TokenType::OR },
    { "not", TokenType::NOT },         { "in", TokenType::IN },
    { "is", TokenType::IS },           { "null", TokenType::NULL_ },
    { "if", TokenType::IF },           { "exists", TokenType::EXISTS },
    { "true", TokenType::TRUE },       { "false", TokenType::FALSE },
    { "between", TokenType::BETWEEN }, { "distinct", TokenType::DISTINCT },
    { "all", TokenType::ALL }
};

Token Lexer::getLiteralToken() {
    Token token(TokenType::ILLEGAL, "");
    int   len = 1;
    while (std::isalpha(sql[ sqlPos + len ]) ||
           std::isdigit(sql[ sqlPos + len ]) || sql[ sqlPos + len ] == '_') {
        len++;
    }
    std::string word = sql.substr(sqlPos, len);
    toLowerCase(word);
    if (keywordTokenType.count(word) != 0) {
        token = Token(keywordTokenType[ word ], word);
    } else {
        token = Token(TokenType::IDENTIFIER, word);
    }
    sqlPos += len;
    return token;
}

Token Lexer::getNumberToken() {
    Token token(TokenType::INTEGER, "0");
    int   len = 0, numOfDot = 0;
    while (std::isdigit(sql[ sqlPos + len ]) || sql[ sqlPos + len ] == '.') {
        if (sql[ sqlPos + len ] == '.') { numOfDot++; }
        len++;
    }
    if (numOfDot == 0) {
        token = Token(TokenType::INTEGER, sql.substr(sqlPos, len));
    } else if (numOfDot == 1) {
        token = Token(TokenType::FLOAT, sql.substr(sqlPos, len));
    } else {
        token = Token(TokenType::ILLEGAL, sql.substr(sqlPos, len));
    }
    sqlPos += len;
    return token;
}

Token Lexer::scanNextToken() {
    if (sqlPos >= sql.size()) { return Token(TokenType::END, ""); }
    // ingore the space
    while (std::isspace(sql[ sqlPos ])) {
        sqlPos++;
    }
    Token token;
    char  ch = sql[ sqlPos ];
    if (std::isdigit(ch)) {
        token = getNumberToken();
    } else if (std::isalpha(ch)) {
        token = getLiteralToken();
    } else {
        token = getSymbolToken();
    }
    return token;
}

Token Lexer::getNextToken() {
    if (tokenPos < tokenSequence.size()) {
        return tokenSequence[ tokenPos++ ];
    } else {
        return Token(TokenType::END);
    }
}

Token Lexer::getCurrentToken() {
    if (tokenPos < tokenSequence.size()) {
        fmt::print("get current token: {}\n", tokenSequence[ tokenPos ].val);
        return tokenSequence[ tokenPos ];
    } else {
        return Token(TokenType::END);
    }
}

void Lexer::scanTokens() {
    do {
        tokenSequence.push_back(scanNextToken());
    } while (tokenSequence.back().tokenType != TokenType::END);
}
} // namespace minidb::parser