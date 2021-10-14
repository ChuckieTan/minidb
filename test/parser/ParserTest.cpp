#include "SQLColumnDefine.h"
#include "SQLCreateTableStatement.h"
#include <gtest/gtest.h>
#include <iostream>

#define protected public
#define private public

#include "Lexer.h"
#include "Parser.h"
#include "TokenType.h"

namespace {
TEST(Parser, CreateTableStatement) {
    using namespace minidb::parser;

    Parser parser("create table student (id int, name text, address text);");
    minidb::ast::SQLCreateTableStatement statement =
        parser.createTableStatement();
    EXPECT_EQ(statement.tableName, "student");
    std::cerr << statement.columnDefineList[ 0 ].columnName << std::endl;
    std::cerr
        << "----------statement.columnDefineList[ 0 ].columnName----------"
        << std::endl;
    std::cerr << statement.columnDefineList[ 1 ].columnName << std::endl;
    std::cerr
        << "----------statement.columnDefineList[ 1 ].columnName----------"
        << std::endl;
    EXPECT_EQ(
        statement.columnDefineList,
        std::vector<minidb::ast::SQLColumnDefine>(
            { minidb::ast::SQLColumnDefine(
                  "id", minidb::ast::SQLColumnDefine::ColumnType::INT),
              minidb::ast::SQLColumnDefine(
                  "name", minidb::ast::SQLColumnDefine::ColumnType::TEXT),
              minidb::ast::SQLColumnDefine(
                  "address", minidb::ast::SQLColumnDefine::ColumnType::TEXT) }));
}
} // namespace