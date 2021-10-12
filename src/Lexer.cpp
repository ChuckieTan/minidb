#include "Lexer.h"
#include "Token.h"
#include "TokenType.h"
#include <algorithm>
#include <cctype>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace minidb {

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
    : pos(0)
    , sql(_sql) {
}

Lexer::Lexer(const std::string &_sql)
    : pos(0)
    , sql(_sql) {
}

Lexer::SavePoint Lexer::mark() {
    SavePoint savePoint;
    savePoint.pos = pos;
    return savePoint;
}

void Lexer::reset(int _pos) {
    pos = _pos;
}

void Lexer::reset(SavePoint savePoint) {
    pos = savePoint.pos;
}

Token Lexer::getSymbolToken() {
    Token token;
    if (auto ch = sql.substr(pos, 2); symbolTokenType.count(ch)) {
        token = Token(symbolTokenType[ ch ], ch);
        pos += 2;
    } else if (auto ch = sql.substr(pos, 1); symbolTokenType.count(ch)) {
        token = Token(symbolTokenType[ ch ], ch);
        pos += 1;
    } else {
        if (sql[ pos ] == '\'') {
            pos++;
            int len = 0;
            while (
                !(sql[ pos + len ] == '\'' && sql[ pos + len - 1 ] != '\\')) {
                len++;
            }
            token = Token(TokenType::STRING, sql.substr(pos, pos + len));
            pos += len + 1;
        } else {
            token = Token(TokenType::ILLEGAL, sql.substr(pos, 1));
            pos += 1;
        }
    }
    return token;
}

void Lexer::toLowerCase(std::string &str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

std::unordered_map<std::string, TokenType> Lexer::keywordTokenType = {
    { "create", TokenType::CREATE },
    { "table", TokenType::TABLE },
    { "insert", TokenType::INSERT },
    { "into", TokenType::INTO },
    { "delete", TokenType::DELETE },
    { "drop", TokenType::DROP },
    { "select", TokenType::SELECT },
    { "from", TokenType::FROM },
    { "where", TokenType::WHERE },
    { "and", TokenType::AND },
    { "or", TokenType::OR },
    { "not", TokenType::NOT },
    { "in", TokenType::IN },
    { "is", TokenType::IS },
    { "null", TokenType::NULL_ },
    { "if", TokenType::IF },
    { "exists", TokenType::EXISTS },
    { "true", TokenType::TRUE },
    { "false", TokenType::FALSE },
    { "between", TokenType::BETWEEN },
    { "distinct", TokenType::DISTINCT },
    { "all", TokenType::ALL }
};

Token Lexer::getLiteralToken() {
    Token token(TokenType::ILLEGAL, "");
    int   len = 1;
    while (std::isalpha(sql[ pos + len ]) || std::isdigit(sql[ pos + len ]) ||
           sql[ pos + len ] == '_') {
        len++;
    }
    std::string word = sql.substr(pos, len);
    toLowerCase(word);
    if (keywordTokenType.count(word) != 0) {
        token = Token(keywordTokenType[ word ], word);
    } else {
        token = Token(TokenType::IDENTIFIER, word);
    }
    pos += len;
    return token;
}

Token Lexer::getNumberToken() {
    Token token(TokenType::INTEGER, "0");
    int   len = 0, numOfDot = 0;
    while (std::isdigit(sql[ pos + len ]) || sql[ pos + len ] == '.') {
        if (sql[ pos + len ] == '.') {
            numOfDot++;
        }
        len++;
    }
    if (numOfDot == 0) {
        token = Token(TokenType::INTEGER, sql.substr(pos, len));
    } else if (numOfDot == 1) {
        token = Token(TokenType::FLOAT, sql.substr(pos, len));
    } else {
        token = Token(TokenType::ILLEGAL, sql.substr(pos, len));
    }
    pos += len;
    return token;
}

Token Lexer::getNextToken() {
    if (pos >= sql.size()) {
        return Token(TokenType::END, "");
    }
    // ingore the space
    while (std::isspace(sql[ pos ])) {
        pos++;
    }
    Token token;
    char  ch = sql[ pos ];
    if (std::isdigit(ch)) {
        token = getNumberToken();
    } else if (std::isalpha(ch)) {
        token = getLiteralToken();
    } else {
        token = getSymbolToken();
    }
    return token;
}
} // namespace minidb