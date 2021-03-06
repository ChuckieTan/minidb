#include "Parser.h"
#include "Lexer.h"
#include "SQLColumnAssign.h"
#include "SQLColumnDefine.h"
#include "SQLCreateTableStatement.h"
#include "SQLDeleteStatement.h"
#include "SQLDropTableStatement.h"
#include "SQLExpr.h"
#include "SQLExprValue.h"
#include "SQLInsertIntoStatement.h"
#include "SQLSelectStatement.h"
#include "SQLUpdateStatement.h"
#include "SQLWhereStatement.h"
#include "Token.h"
#include "TokenType.h"
#include "spdlog/fmt/bundled/core.h"
#include <functional>
#include <initializer_list>
#include <iostream>
#include <string>
#include <variant>

#include <spdlog/spdlog.h>

#define FUNC(fun) std::function([ & ]() -> bool { return fun(); })

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
        if (!chain({ TokenType::RBRACKET, TokenType::SEMICOLON })) {
            spdlog::error("expected ')' or ';'");
        }
    } else {
        spdlog::error("not a create table statement");
    }
    return statement;
}

ast::SQLDropTableStatement Parser::parseDropTableStatement() {
    ast::SQLDropTableStatement statement;
    if (chain({ TokenType::DROP, TokenType::TABLE })) {
        if (chain({ TokenType::IF, TokenType::EXISTS })) {
            statement.ifExists = true;
        } else {
            statement.ifExists = false;
        }
        statement.tableName = tableName();
        if (!chain({ TokenType::SEMICOLON })) {
            spdlog::error("expected ';'");
        }
    } else {
        spdlog::error("not a drop table statement");
    }
    return statement;
}

ast::SQLInsertIntoStatement Parser::parseInsertIntoStatement() {
    ast::SQLInsertIntoStatement statement;
    if (chain({ TokenType::INSERT, TokenType::INTO })) {
        if (auto token = lexer.getNextToken();
            token.tokenType == TokenType::IDENTIFIER) {
            statement.tableName = token.val;
        } else {
            spdlog::error("expected a table name, given '{}'", token.val);
        }
        if (chain({ TokenType::VALUES, TokenType::LBRACKET })) {
            do {
                statement.values.push_back(literalValue());
            } while (match(TokenType::COMMA));
        } else {
            spdlog::error("expected 'values' or '('");
        }
        if (!chain({ TokenType::RBRACKET, TokenType::SEMICOLON })) {
            spdlog::error("expected ')' or ';'");
        }
    } else {
        spdlog::error("not a insert into statement");
    }
    return statement;
}

ast::SQLSelectStatement Parser::parseSelectStatement() {
    ast::SQLSelectStatement statement;
    if (match(TokenType::SELECT)) {
        if (match(TokenType::STAR)) {
            statement.resultList.push_back("*");
        } else {
            do {
                statement.resultList.push_back(columnName());
            } while (match(TokenType::COMMA));
        }

        if (auto token = lexer.getCurrentToken(); !match(TokenType::FROM)) {
            spdlog::error("expected a from, given '{}'", token.val);
        }

        if (auto token = lexer.getCurrentToken();
            match(TokenType::IDENTIFIER)) {
            statement.tableSource = token.val;
        } else {
            spdlog::error("expected a table source, given '{}'", token.val);
        }

        if (auto token = lexer.getCurrentToken();
            token.tokenType == TokenType::WHERE) {
            statement.where         = parseWhere();
            statement.isWhereExists = true;
        } else {
            statement.isWhereExists = false;
        }

        if (!chain({ TokenType::SEMICOLON, TokenType::END })) {
            spdlog::error("expected a table semicolon in the end");
        }
    } else {
        spdlog::error("not a select statement");
    }
    return statement;
}

ast::SQLUpdateStatement Parser::parseUpdateStatement() {
    ast::SQLUpdateStatement statement;
    if (match(TokenType::UPDATE)) {
        if (auto token = lexer.getNextToken();
            token.tokenType == TokenType::IDENTIFIER) {
            statement.tableSource = token.val;
        } else {
            spdlog::error("expected a table name, given '{}'", token.val);
        }

        if (auto token = lexer.getCurrentToken(); !match(TokenType::SET)) {
            spdlog::error("expected keyword set, given '{}'", token.val);
        }

        do {
            statement.columnAssign.push_back(columnAssign());
        } while (match(TokenType::COMMA));

        if (auto token = lexer.getCurrentToken();
            token.tokenType == TokenType::WHERE) {
            statement.where         = parseWhere();
            statement.isWhereExists = true;
        } else {
            statement.isWhereExists = false;
        }

        if (!chain({ TokenType::SEMICOLON, TokenType::END })) {
            spdlog::error("expected a table semicolon in the end");
        }
    } else {
        spdlog::error("not a update statement");
    }
    return statement;
}

ast::SQLDeleteStatement Parser::parseDeleteStatement() {
    ast::SQLDeleteStatement statement;
    if (chain({ TokenType::DELETE, TokenType::FROM })) {
        if (auto token = lexer.getCurrentToken();
            match(TokenType::IDENTIFIER)) {
            statement.tableSource = token.val;
        } else {
            spdlog::error("expected a table name, given '{}'", token.val);
        }

        if (auto token = lexer.getCurrentToken();
            token.tokenType == TokenType::WHERE) {
            statement.where         = parseWhere();
            statement.isWhereExists = true;
        } else {
            statement.isWhereExists = false;
        }

        if (!chain({ TokenType::SEMICOLON, TokenType::END })) {
            spdlog::error("expected a table semicolon in the end");
        }
    } else {
        spdlog::error("not a delete statement");
    }
    return statement;
}

ast::SQLColumnAssign Parser::columnAssign() {
    ast::SQLColumnAssign columnAssign;
    if (auto token = lexer.getCurrentToken(); match(TokenType::IDENTIFIER)) {
        columnAssign.columnName = token.val;
    } else {
        spdlog::error("expected a column name, given '{}'", token.val);
    }

    if (auto token = lexer.getCurrentToken(); !match(TokenType::ASSIGN)) {
        spdlog::error("expected '=', given '{}'", token.val);
    }

    columnAssign.value = exprValue();
    return columnAssign;
}

std::string Parser::columnName() {
    if (auto token = lexer.getCurrentToken(); match(TokenType::IDENTIFIER)) {
        return token.val;
    } else {
        spdlog::error("expected a column name, given '{}'", token.val);
        return "";
    }
}

ast::SQLExpr Parser::parseExpr() {
    auto         token = lexer.getCurrentToken();
    ast::SQLExpr expr;
    expr.lValue = exprValue();
    expr.op     = comparisonOperator();
    expr.rValue = exprValue();
    return expr;
}

ast::SQLWhereStatement Parser::parseWhere() {
    ast::SQLWhereStatement statement;
    if (auto token = lexer.getNextToken();
        token.tokenType == TokenType::WHERE) {
        statement.expr = parseExpr();
    } else {
        spdlog::error("expected a where, given '{}'", token.val);
    }
    return statement;
}

TokenType Parser::comparisonOperator() {
    if (auto token = lexer.getCurrentToken();
        tree({ TokenType::LESS, TokenType::LESS_OR_EQUAL, TokenType::ASSIGN,
               TokenType::EQUAL, TokenType::NOT_EQUAL, TokenType::GREATER,
               TokenType::GREATER_OR_EQUAL })) {
        return token.tokenType;
    } else {
        spdlog::error("given '{}'", lexer.getCurrentToken().tokenType);
        spdlog::error("expexted a comparison operator, given '{}'", token.val);
        return TokenType::ILLEGAL;
    }
}

ast::SQLExprValue Parser::exprValue() {
    auto              token = lexer.getNextToken();
    int               sign  = 1;
    ast::SQLExprValue value;
    if (token.tokenType == TokenType::STRING) {
        value = ast::SQLExprValue(token.val);
    } else if (token.tokenType == TokenType::IDENTIFIER) {
        value =
            ast::SQLExprValue(token.val, ast::SQLExprValue::DataType::COLUMN);
    } else if (token.tokenType == TokenType::PLUS) {
        sign  = 1;
        value = numericValue(sign);
    } else if (token.tokenType == TokenType::MINUS) {
        sign  = -1;
        value = numericValue(sign);
    } else {
        value = numericValue(1, token);
    }
    return value;
}

ast::SQLExprValue Parser::numericValue(int sign, Token token) {
    if (token.tokenType == TokenType::ILLEGAL) { token = lexer.getNextToken(); }
    ast::SQLExprValue value;
    if (token.tokenType == TokenType::INTEGER) {
        value = ast::SQLExprValue(sign * std::stol(token.val));
    } else if (token.tokenType == TokenType::FLOAT) {
        value = ast::SQLExprValue(sign * std::stod(token.val));
    } else {
        spdlog::error("expected a value, given '{}'", token.val);
    }
    return value;
}

ast::SQLExprValue Parser::literalValue() {
    auto              token = lexer.getNextToken();
    int               sign  = 1;
    ast::SQLExprValue value;
    if (token.tokenType == TokenType::STRING) {
        value = ast::SQLExprValue(token.val);
    } else if (token.tokenType == TokenType::PLUS) {
        sign  = 1;
        value = numericValue(sign);
    } else if (token.tokenType == TokenType::MINUS) {
        sign  = -1;
        value = numericValue(sign);
    } else {
        value = numericValue(1, token);
    }
    return value;
}

std::string Parser::tableName() {
    if (lexer.getCurrentToken().tokenType != TokenType::IDENTIFIER) {
        spdlog::error("expected a table name");
        return "";
    }
    return lexer.getNextToken().val;
}

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
    bool res       = false;
    if (condition.isToken()) {
        res = lexer.getNextToken().tokenType == condition.getToken();
    } else if (condition.isFunc()) {
        res = condition.getFunc()();
    } else if (condition.isString()) {
        res = lexer.getNextToken().val == condition.getString();
    }
    if (!res) { lexer.reset(savePoint); }
    return res;
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
    auto savePoint = lexer.mark();
    for (const auto &condition : args) {
        if (match(condition)) {
            return true;
        } else {
            lexer.reset(savePoint);
        }
    }
    return false;
}

int Parser::parseStatement() {
    int ans = 0;
    if (auto tokenType = lexer.getCurrentToken().tokenType;
    tokenType == TokenType::CREATE) {
        parseCreateTableStatement();
        ans = 1;
    } else if (tokenType == TokenType::SELECT) {
        parseSelectStatement();
        ans = 2;
    } else if (tokenType == TokenType::INSERT) {
        parseInsertIntoStatement();
        ans = 3;
    } else if (tokenType == TokenType::UPDATE) {
        parseUpdateStatement();
        ans = 4;
    } else if (tokenType == TokenType::DELETE) {
        parseDeleteStatement();
        ans = 5;
    }
    return ans;
}

} // namespace minidb::parser