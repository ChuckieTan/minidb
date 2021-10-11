#include "Parser.h"
#include "Lexer.h"
#include "TokenType.h"
#include <functional>
#include <initializer_list>
#include <iostream>
#include <string>
#include <variant>

#define FUNC(fun) std::function([ & ]() -> bool { return fun(); })

#ifdef DEBUG
int indent = 0;
#endif

namespace minidb {

Parser::MatchType::MatchType(const std::string &str) {
    _data    = str;
    dataType = DataType::STRING;
}
Parser::MatchType::MatchType(const TokenType tokenType) {
    _data    = tokenType;
    dataType = DataType::TOKEN;
}
Parser::MatchType::MatchType(const std::function<bool()> &func) {
    _data    = func;
    dataType = DataType::FUNC;
}

bool Parser::MatchType::isString() {
    return dataType == DataType::STRING;
}
bool Parser::MatchType::isToken() {
    return dataType == DataType::TOKEN;
}
bool Parser::MatchType::isFunc() {
    return dataType == DataType::FUNC;
}

std::string Parser::MatchType::getString() {
    return std::get<std::string>(_data);
}
std::function<bool()> Parser::MatchType::getFunc() {
    return std::get<std::function<bool()>>(_data);
}
TokenType Parser::MatchType::getToken() {
    return std::get<TokenType>(_data);
}

Parser::Parser(const Lexer &_lexer)
    : lexer(_lexer) {
}

Parser::Parser(std::string &&_sql)
    : lexer(_sql) {
}

Parser::Parser(const std::string &_sql)
    : lexer(_sql) {
}

bool Parser::match(MatchType &condition) {
    if (condition.isToken()) {
        return lexer.getNextToken().tokenType == condition.getToken();
    } else if (condition.isFunc()) {
        return condition.getFunc()();
    } else if (condition.isString()) {
        return lexer.getNextToken().val == condition.getString();
    }
    return false;
}

bool Parser::chain(std::initializer_list<MatchType> args) {
    auto savePoint = lexer.mark();
    for (auto condition : args) {
        if (!match(condition)) {
            lexer.reset(savePoint);
            return false;
        }
    }
    return true;
}

bool Parser::selectStatement() {
    bool res = chain({ TokenType::SELECT, FUNC(selectList), TokenType::FROM,
                       FUNC(table) }) &&
               many({ FUNC(whereStatement) }) &&
               chain({ TokenType::SEMICOLON });
    std::cout << "selectStatement\t" << (res ? "true" : "false") << "\n";
    return res;
}

bool Parser::selectList() {
    bool res = field() && many({ TokenType::COMMA, FUNC(field) });
    std::cout << "selectList\t" << (res ? "true" : "false") << "\n";
    return res;
}

bool Parser::table() {
    bool res = lexer.getNextToken().tokenType == TokenType::IDENTIFIER;
    std::cout << "table\t\t" << (res ? "true" : "false") << "\n";
    return res;
}

bool Parser::whereStatement() {
    return false;
}

bool Parser::columnName() {
    bool res = tree({ FUNC(functional), TokenType::STAR, FUNC(identifier) });
    std::cout << "colunmName\t" << (res ? "true" : "false") << "\n";
    return res;
}

bool Parser::identifier() {
    bool res = lexer.getNextToken().tokenType == TokenType::IDENTIFIER;
    std::cout << "identifier\t" << (res ? "true" : "false") << "\n";
    return res;
}

bool Parser::functional() {
    bool res = chain({ FUNC(identifier), TokenType::LBRACKET, FUNC(columnName),
                       TokenType::RBRACKET });
    std::cout << "functional\t" << (res ? "true" : "false") << "\n";
    return res;
}
bool Parser::field() {
    bool res = tree({ FUNC(functional), FUNC(identifier), TokenType::STAR });
    std::cout << "colunmName\t" << (res ? "true" : "false") << "\n";
    return res;
}

bool Parser::many(std::initializer_list<MatchType> args) {
    auto savePoint = lexer.mark();
    while (chain(args)) {
        savePoint = lexer.mark();
    }
    lexer.reset(savePoint);
    return true;
}

bool Parser::tree(std::initializer_list<MatchType> args) {
    auto savePoint = lexer.mark();
    for (auto condition : args) {
        if (match(condition)) {
            return true;
        }
        lexer.reset(savePoint);
    }
    return false;
}

} // namespace minidb