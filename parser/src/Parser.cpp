#include "Parser.h"
#include "Lexer.h"
#include "SQLColumnDefine.h"
#include "SQLCreateTableStatement.h"
#include "SQLDropTableStatement.h"
#include "Token.h"
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

bool Parser::MatchType::isString() { return dataType == DataType::STRING; }
bool Parser::MatchType::isToken() { return dataType == DataType::TOKEN; }
bool Parser::MatchType::isFunc() { return dataType == DataType::FUNC; }

std::string Parser::MatchType::getString() {
    return std::get<std::string>(_data);
}
std::function<bool()> Parser::MatchType::getFunc() {
    return std::get<std::function<bool()>>(_data);
}
TokenType Parser::MatchType::getToken() { return std::get<TokenType>(_data); }

Parser::Parser(const Lexer &_lexer)
    : lexer(_lexer) {}

Parser::Parser(std::string &&_sql)
    : lexer(_sql) {}

Parser::Parser(const std::string &_sql)
    : lexer(_sql) {}

ast::SQLCreateTableStatement Parser::parseCreateTableStatement() {
    ast::SQLCreateTableStatement statement;
    if (chain({ TokenType::CREATE, TokenType::TABLE })) {
        statement.tableName = tableName();
        if (match(TokenType::LBRACKET)) {
            do {
                statement.columnDefineList.push_back(columnDefine());
            } while (match(TokenType::COMMA));
        }
        if (!(match(TokenType::RBRACKET) && match(TokenType::SEMICOLON))) {
            spdlog::error("expected ')'");
        }
    } else {
        spdlog::error("not a create table statement");
    }
    return statement;
}

ast::SQLDropTableStatement Parser::parseDropTableStatement() {
    ast::SQLDropTableStatement statement;
    if (chain({ TokenType::DROP, TokenType::TABLE })) {
        if (chain({ TokenType::NOT, TokenType::EXISTS })) {
            statement.ifExists = true;
        } else {
            statement.ifExists = false;
        }
        statement.tableName = tableName();
    }
    return statement;
}

std::string Parser::tableName() {
    if (lexer.getCurrentToken().tokenType != TokenType::IDENTIFIER) {
        spdlog::error("expected a table name");
        return "";
    }
    return lexer.getNextToken().val;
}

ast::SQLTableElement Parser::tableElement() {}

ast::SQLColumnDefine Parser::columnDefine() {
    if (lexer.getCurrentToken().tokenType != TokenType::IDENTIFIER) {
        spdlog::error("expected a cloumn name");
    }

    ast::SQLColumnDefine define;
    define.columnName = lexer.getNextToken().val;
    Token token       = lexer.getNextToken();

    if (token.tokenType != TokenType::IDENTIFIER) {
        spdlog::error("expected a cloumn name");
    } else if (token.val != "int" && token.val != "float" &&
               token.val != "text") {
        spdlog::error("invalid column datatype: {}", token.val);
    } else {
        define.setColumnType(token.val);
    }
    return define;
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
    auto savePoint = lexer.mark();
    for (const auto &condition : args) {
        if (!match(condition)) {
            lexer.reset(savePoint);
            return false;
        }
    }
    return true;
}

bool Parser::optional(std::initializer_list<MatchType> args) {
    if (chain(args)) {}
    return true;
}

bool Parser::many(std::initializer_list<MatchType> args) {
    while (chain(args)) {}
    return true;
}

bool Parser::tree(std::initializer_list<MatchType> args) {
    for (const auto &condition : args) {
        if (match(condition)) { return true; }
    }
    return false;
}

} // namespace minidb::parser