#include "Tokenizer.h"
#include "Token.h"
#include "TokenType.h"
#include <cctype>
#include <string>
#include <unordered_map>

namespace minidb {

std::unordered_map<char, Token> Tokenizer::singleCharacterToken = {
    { ',', Token(TokenType::COMMA, ",") },
    { '(', Token(TokenType::LBRACKET, "(") },
    { ')', Token(TokenType::RBRACKET, ")") },
    { '+', Token(TokenType::PLUS, "+") },
    { '-', Token(TokenType::MINUS, "-") },
    { ';', Token(TokenType::SEMICOLON, ";") }
};

std::unordered_map<std::string, Token> Tokenizer::doubleCharacterToken = {
    { "!=", Token(TokenType::NOT_EQUAL, "!=") },
    { "<=", Token(TokenType::LESS_OR_EQUAL, "<=") },
    { "<>", Token(TokenType::NOT_EQUAL, "<>") },
    { ">=", Token(TokenType::GREATER_OR_EQUAL, ">=") },
    { "==", Token(TokenType::EQUAL, "==") }
};

Tokenizer::Tokenizer(const std::string &_sql)
    : sql(_sql)
    , pos(0) {
}

Token Tokenizer::getSymbolToken() {
    std::string ch(1, sql[ pos ]);
    if (pos != sql.size() - 1) {
        ch += sql[ pos + 1 ];
    }
    Token token;
    if (doubleCharacterToken.find(ch) != doubleCharacterToken.end()) {
        pos += 2;
        token = doubleCharacterToken[ ch ];
    } else if (singleCharacterToken.find(ch[ 0 ]) !=
               singleCharacterToken.end()) {
        pos++;
        token = singleCharacterToken[ ch[ 0 ] ];
    } else {
        pos++;
        token = Token(TokenType::ILLEGAL, std::string(1, sql[pos]));
    }
    return token;
}

Token Tokenizer::getLiteralToken() {
    return Token(TokenType::ILLEGAL, "");
}

Token Tokenizer::getNumberToken() {
    return Token(TokenType::ILLEGAL, "");
}

Token Tokenizer::getNextToken() {
    if (pos == sql.size()) {
        return Token(TokenType::END, "");
    }
    // ingore the space
    while (std::isspace(sql[ pos++ ])) {
        pos++;
    }
    Token token;
    char ch = sql[pos];
    if (std::isdigit(ch)) {
        token = getNumberToken();
    } else if (std::isalpha(ch)) {
        token = getLiteralToken();
    } else {
        token = getLiteralToken();
    }
    return token;
}
} // namespace minidb