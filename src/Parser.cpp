#include "Parser.h"
#include "Lexer.h"
#include "TokenType.h"
#include <functional>
#include <initializer_list>
#include <iostream>
#include <string>
#include <variant>

#define FUNC(fun) std::function([ & ]() -> bool { return fun(); })

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
    for (auto condition : args) {
        if (!match(condition)) {
            std::cout << "false" << std::endl;
            return false;
        }
        if (condition.isFunc()) {
            std::cout << "func\n";
        } else if (condition.isToken()) {
            std::cout << static_cast<int>(condition.getToken()) << "\n";
        }
    }
    std::cout << "true" << std::endl;
    return true;
}

bool Parser::selectStatement() {
    return chain({ TokenType::SELECT, FUNC(selectList), TokenType::FROM,
                   FUNC(table) }) &&
           optional({ FUNC(whereStatement) }) &&
           chain({ TokenType::SEMICOLON });
}

bool Parser::selectList() {
    return field() && optional({ TokenType::COMMA, FUNC(word) });
}

bool Parser::table() {
    return lexer.getNextToken().tokenType == TokenType::IDENTIFIER;
}

bool Parser::whereStatement() {
    return true;
}

bool Parser::word() {
    return lexer.getNextToken().tokenType == TokenType::IDENTIFIER;
}
bool Parser::functional() {
    return chain({FUNC(word), TokenType::LBRACKET, FUNC(word), TokenType::RBRACKET });
}
bool Parser::field() {
    return tree({ FUNC(functional), FUNC(word) });
}

bool Parser::optional(std::initializer_list<MatchType> args) {
    return tree(args) || true;
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