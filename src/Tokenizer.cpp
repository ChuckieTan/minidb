#include "Tokenizer.h"
#include "Token.h"
#include "TokenType.h"
#include <algorithm>
#include <cctype>
#include <string>
#include <unordered_map>

namespace minidb {

std::unordered_map<std::string, TokenType> Tokenizer::symbolTokenType = {
    { ",", TokenType::COMMA },      { "*", TokenType::STAR },
    { "(", TokenType::LBRACKET },   { ")", TokenType::RBRACKET },
    { "+", TokenType::PLUS },       { "-", TokenType::MINUS },
    { ";", TokenType::SEMICOLON },  { "=", TokenType::EQUAL },
    { "!=", TokenType::NOT_EQUAL }, { "<=", TokenType::LESS_OR_EQUAL },
    { "<>", TokenType::NOT_EQUAL }, { ">=", TokenType::GREATER_OR_EQUAL }
};

Tokenizer::Tokenizer(const std::string &_sql)
    : pos(0)
    , sql(_sql) {
}

Token Tokenizer::getSymbolToken() {
    Token token;
    if (auto ch = sql.substr(pos, 2); symbolTokenType.count(ch)) {
        token = Token(symbolTokenType[ ch ]);
        pos += 2;
    } else if (auto ch = sql.substr(pos, 1); symbolTokenType.count(ch)) {
        token = Token(symbolTokenType[ ch ]);
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

void Tokenizer::toLowerCase(std::string &str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

std::unordered_map<std::string, TokenType> Tokenizer::keywordTokenType = {
    { "create", TokenType::CREATE }, { "table", TokenType::TABLE },
    { "insert", TokenType::INSERT }, { "into", TokenType::INTO },
    { "delete", TokenType::DELETE }, { "drop", TokenType::DROP },
    { "select", TokenType::SELECT }, { "from", TokenType::FROM },
    { "where", TokenType::WHERE },   { "and", TokenType::AND },
    { "or", TokenType::OR },         { "not", TokenType::NOT }
};

Token Tokenizer::getIdToken() {
    int  len = 0;
    char ch;
    do {
        len++;
        ch = sql[ pos + len ];
    } while (std::isalpha(ch) || ch == '_');
    pos += len;
    return Token(TokenType::ID, sql.substr(pos - len, len));
}

Token Tokenizer::getLiteralToken() {
    Token       token(TokenType::ILLEGAL, "");
    std::string tmp = sql.substr(pos, 6);
    toLowerCase(tmp);
    int flag = 0;
    for (int i = 0; i <= tmp.size(); i++) {
        if (keywordTokenType.count(tmp) == 1) {
            flag  = 1;
            token = Token(keywordTokenType[ tmp ]);
            pos += tmp.size();
            break;
        }
        tmp.pop_back();
    }
    if (!flag) {
        token = getIdToken();
    }
    return token;
}

Token Tokenizer::getNumberToken() {
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

Token Tokenizer::getNextToken() {
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