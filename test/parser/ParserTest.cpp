#include "SQLColumnDefine.h"
#include "SQLCreateTableStatement.h"
#include "SQLDropTableStatement.h"
#include "SQLExpr.h"
#include "SQLExprValue.h"
#include "SQLInsertIntoStatement.h"
#include "spdlog/spdlog.h"
#include <gtest/gtest.h>
#include <iostream>

#define protected public
#define private public

#include "Lexer.h"
#include "Parser.h"
#include "TokenType.h"

namespace {
TEST(Parser, CreateTableStatement) {
    using namespace minidb;

    parser::Parser parser(
        "create table student (id int, name text, address text);");
    ast::SQLCreateTableStatement statement = parser.parseCreateTableStatement();
    EXPECT_EQ(statement.tableName, "student");
    EXPECT_EQ(
        statement.columnDefineList,
        std::vector<ast::SQLColumnDefine>(
            { ast::SQLColumnDefine("id", ast::SQLColumnDefine::ColumnType::INT),
              ast::SQLColumnDefine("name",
                                   ast::SQLColumnDefine::ColumnType::TEXT),
              ast::SQLColumnDefine("address",
                                   ast::SQLColumnDefine::ColumnType::TEXT) }));
}

TEST(Parser, DropTableStatement) {
    using namespace minidb;

    parser::Parser             parser("drop table student;");
    ast::SQLDropTableStatement statement = parser.parseDropTableStatement();
    EXPECT_EQ(statement.tableName, "student");
    EXPECT_EQ(statement.ifExists, false);
}

TEST(Parser, InsertIntoStatement) {
    using namespace minidb;

    parser::Lexer lexer("insert into student values (123, '123', 1.23);");
    fmt::print("parse: {}\n", lexer.sql);

    parser::Parser parser("insert into student values (123, '123', -1.23);");
    ast::SQLInsertIntoStatement statement = parser.parseInsertIntoStatement();
    EXPECT_EQ(statement.tableName, "student");
    EXPECT_EQ(statement.values,
              std::vector<ast::SQLExprValue>({ ast::SQLExprValue(123),
                                               ast::SQLExprValue("123"),
                                               ast::SQLExprValue(-1.23) }));
}

TEST(Parser, SQLExpr) {
    using namespace minidb;

    parser::Lexer lexer("id = 123");
    fmt::print("parse {}\n", lexer.sql);

    parser::Parser parser(lexer.sql);
    ast::SQLExpr   statement = parser.parseExpr();

    EXPECT_EQ(statement.lValue.getColumnValue(), "id");
    EXPECT_EQ(statement.lValue.isColumn(), true);
    EXPECT_EQ(statement.op, parser::TokenType::ASSIGN);
    EXPECT_EQ(statement.rValue.getIntValue(), 123);
    EXPECT_EQ(statement.rValue.isInt(), true);
}

TEST(Parser, SelectStatement) {
    using namespace minidb;

    parser::Lexer lexer("select * from student where id = 123;");
    fmt::print("parse {}\n", lexer.sql);

    parser::Parser parser(lexer.sql);
    auto statement = parser.parseSelectStatement();

    EXPECT_EQ(statement.resultList, std::vector<std::string>{"*"});
    EXPECT_EQ(statement.tableSource, "student");
    EXPECT_EQ(statement.isWhereExists, true);

    EXPECT_EQ(statement.where.expr.lValue.getColumnValue(), "id");
    EXPECT_EQ(statement.where.expr.lValue.isColumn(), true);
    EXPECT_EQ(statement.where.expr.op, parser::TokenType::ASSIGN);
    EXPECT_EQ(statement.where.expr.rValue.getIntValue(), 123);
    EXPECT_EQ(statement.where.expr.rValue.isInt(), true);

    lexer = parser::Lexer("select id, name, number from student where id = 123;");
    fmt::print("parse {}\n", lexer.sql);

    for (auto token = lexer.getNextToken();
         token.tokenType != parser::TokenType::END &&
         token.tokenType != parser::TokenType::ILLEGAL;
         token = lexer.getNextToken()) {
        fmt::print("{:<4} {}\n", token.tokenType, token.val);
    }
    
    parser = parser::Parser(lexer.sql);
    statement = parser.parseSelectStatement();

    EXPECT_EQ(statement.resultList, std::vector<std::string>({ "id", "name", "number" }));
    EXPECT_EQ(statement.tableSource, "student");
    EXPECT_EQ(statement.isWhereExists, true);

    EXPECT_EQ(statement.where.expr.lValue.getColumnValue(), "id");
    EXPECT_EQ(statement.where.expr.lValue.isColumn(), true);
    EXPECT_EQ(statement.where.expr.op, parser::TokenType::ASSIGN);
    EXPECT_EQ(statement.where.expr.rValue.getIntValue(), 123);
    EXPECT_EQ(statement.where.expr.rValue.isInt(), true);
}
} // namespace