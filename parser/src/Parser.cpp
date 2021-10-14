#include "Parser.h"
#include "Lexer.h"
#include "SQLCreateTableStatement.h"
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

ast::SQLCreateTableStatement Parser::createTableStatement() {
    ast::SQLCreateTableStatement statement;
    if(chain({TokenType::CREATE, TokenType::TABLE})) {
        statement.tableName = tableName();
        if(match(TokenType::LBRACKET)) {
            do {
                statement.columnDefineList.push_back(columnDefine());
            } while(match(TokenType::COMMA));
        }
    } else {
        spdlog::error("not a create table statement");
    }
    return statement;
}



std::string Parser::tableName() {

}

ast::SQLTableElement Parser::tableElement() {

}

ast::SQLColumnDefine Parser::columnDefine() {

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