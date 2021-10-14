#include "Parser.h"
#include "Lexer.h"
#include "TokenType.h"
#include <functional>
#include <initializer_list>
#include <iostream>
#include <string>
#include <variant>

#include <spdlog/spdlog.h>

#define FUNC(fun) std::function([ & ]() -> bool { return fun(); })

#ifdef DEBUG
int indent = 0;
#endif

namespace minidb::parser {

Parser::MatchType::MatchType(const std::string &str) {
    _data    = str;
    dataType = DataType::STRING;
}
Parser::MatchType::MatchType(TokenType &&tokenType) {
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

bool Parser::selectStatement() {
    bool res = chain({ TokenType::SELECT, FUNC(selectList), TokenType::FROM,
                       FUNC(table) }) &&
               optional({ FUNC(whereExpression) }) &&
               chain({ TokenType::SEMICOLON });
    return res;
}

bool Parser::selectList() {
    bool res = resultColumn() && many({ TokenType::COMMA, FUNC(resultColumn) });
    return res;
}

bool Parser::table() {
    bool res = match(TokenType::IDENTIFIER);
    return res;
}

bool Parser::columnNameWithTable() {
    auto savePoint = lexer.mark();
    bool res       = optional({ FUNC(table), TokenType::DOT }) &&
               match(TokenType::IDENTIFIER);
    if (!res) {
        lexer.reset(savePoint);
    }
    return res;
}

bool Parser::expression() {
    bool res = true;
    return res;
}

bool Parser::expressionLValue() {
    bool res = chain({ FUNC(literalValue), FUNC(columnNameWithTable) });
    return res;
}

bool Parser::expressionRValue() {
    return expressionLValue();
}

bool Parser::compareOperator() {
    bool res =
        tree({ TokenType::LESS, TokenType::LESS_OR_EQUAL, TokenType::GREATER,
               TokenType::GREATER_OR_EQUAL, TokenType::ASSIGN, TokenType::EQUAL,
               TokenType::NOT_EQUAL });
    return res;
}

bool Parser::logicalOperator() {
    bool res = tree({ TokenType::AND, TokenType::OR });
}

bool Parser::numbericLiteral() {
    auto savePoint = lexer.mark();
    bool res       = optional({ TokenType::MINUS });
    res = res && (match(TokenType::INTEGER) || match(TokenType::FLOAT));
    if (!res) {
        lexer.reset(savePoint);
    }
    return res;
}

bool Parser::literalValue() {
    bool res = numbericLiteral() || match(TokenType::STRING) ||
               match(TokenType::NULL_) || match(TokenType::TRUE) ||
               match(TokenType::FALSE);
    return res;
}

bool Parser::whereExpression() {
    bool res = chain({ TokenType::WHERE, FUNC(expression) });
    return res;
}

bool Parser::columnName() {
    bool res = tree({ FUNC(functional), TokenType::STAR, FUNC(identifier) });
    return res;
}

bool Parser::identifier() {
    bool res = match(TokenType::IDENTIFIER);
    return res;
}

bool Parser::functional() {
    bool res = chain({ FUNC(identifier), TokenType::LBRACKET, FUNC(columnName),
                       TokenType::RBRACKET });
    return res;
}
bool Parser::resultColumn() {
    bool res = tree({ FUNC(functional), FUNC(identifier), TokenType::STAR });
    return res;
}

bool Parser::match(MatchType condition) {
    auto savePoint = lexer.mark();
    if (condition.isToken()) {
        return lexer.getNextToken().tokenType == condition.getToken();
    } else if (condition.isFunc()) {
        return condition.getFunc()();
    } else if (condition.isString()) {
        return lexer.getNextToken().val == condition.getString();
    }
    lexer.reset(savePoint);
    return false;
}

bool Parser::chain(std::initializer_list<MatchType> args) {
    for (const auto &condition : args) {
        if (!match(condition)) {
            return false;
        }
    }
    return true;
}

bool Parser::optional(std::initializer_list<MatchType> args) {
    if (chain(args)) {
    }
    return true;
}

bool Parser::many(std::initializer_list<MatchType> args) {
    while (chain(args)) {
    }
    return true;
}

bool Parser::tree(std::initializer_list<MatchType> args) {
    for (const auto &condition : args) {
        if (match(condition)) {
            return true;
        }
    }
    return false;
}

} // namespace minidb