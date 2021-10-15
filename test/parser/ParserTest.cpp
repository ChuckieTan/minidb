#include "SQLColumnDefine.h"
#include "SQLCreateTableStatement.h"
#include "SQLDropTableStatement.h"
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
    fmt::print("{}\n", lexer.sql);

    for (auto token = lexer.getNextToken();
         token.tokenType != parser::TokenType::END &&
         token.tokenType != parser::TokenType::ILLEGAL;
         token = lexer.getNextToken()) {
        fmt::print("{:<4} {}\n", token.tokenType, token.val);
    }
    parser::Parser parser("insert into student values (123, '123', -1.23);");
    ast::SQLInsertIntoStatement statement = parser.parseInsertIntoStatement();
    EXPECT_EQ(statement.tableName, "student");
    EXPECT_EQ(statement.values,
              std::vector<ast::SQLExprValue>({ ast::SQLExprValue(123),
                                               ast::SQLExprValue("123"),
                                               ast::SQLExprValue(-1.23) }));
}
} // namespace