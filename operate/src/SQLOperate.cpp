#include "SQLOperate.h"
#include "BPlusTree.h"
#include "BinaryOperate.h"
#include "Lexer.h"
#include "Parser.h"
#include "TokenType.h"
#include "spdlog/fmt/bundled/core.h"
#include "spdlog/spdlog.h"
#include <iostream>

namespace minidb::operate {

SQLOperate::SQLOperate(std::string &&_file_name)
    : file_name(_file_name)
    , parser("")
    , lexer("")
    , storage(_file_name, false) {}

void SQLOperate::main_loop() {
    // for(const auto &data: *storage.table_info_map["student"].b_plus_tree) {
    //     auto expr_list = BinaryOperate::load(data);
    //     for(const auto& expr: expr_list) {
    //         print_expr(expr);
    //     }
    //     fmt::print("\n");
    // }
    input.readInputFromStream(std::cin);
    while (input.buffer != ".quit;") {
        lexer  = parser::Lexer(input.buffer);
        parser = parser::Parser(lexer);
        if (lexer.getCurrentToken().tokenType == parser::TokenType::CREATE) {
            create_table_operate();
        } else if (lexer.getCurrentToken().tokenType ==
                   parser::TokenType::INSERT) {
            insert_operate();
        } else if (lexer.getCurrentToken().tokenType ==
                   parser::TokenType::SELECT) {
            select_operate();
        } else if (lexer.getCurrentToken().tokenType ==
                   parser::TokenType::DELETE) {
            delete_operate();
        }
        input.readInputFromStream(std::cin);
    }
}

bool SQLOperate::create_table_operate() {
    auto statement = parser.parseCreateTableStatement();
    bool ok        = storage.new_table(statement);
    if (ok) { spdlog::info("create table {} successful", statement.tableName); }
    return ok;
}

bool SQLOperate::insert_operate() {
    auto statement = parser.parseInsertIntoStatement();
    auto data      = BinaryOperate::dump(statement.values);
    bool ok        = storage.insert_data(statement.tableName,
                                  statement.values[ 0 ].getIntValue(), data);
    if (ok) { spdlog::info("insert successful"); }
    return ok;
}

bool SQLOperate::select_operate() {
    auto statement = parser.parseSelectStatement();
    if (statement.isWhereExists) {
        if (statement.where.expr.lValue.getStringValue() == "id" &&
            statement.where.expr.op == parser::TokenType::ASSIGN) {
            auto data =
                storage.search_data(statement.tableSource,
                                    statement.where.expr.rValue.getIntValue());
            auto expr_list = BinaryOperate::load(data);
            for (const auto &v : expr_list) {
                print_expr(v);
            }
            fmt::print("\n");
        } else {
        }
    } else {
    }
    return true;
}

bool SQLOperate::delete_operate() {
    bool res       = false;
    auto statement = parser.parseDeleteStatement();
    if (statement.isWhereExists) {
        if (statement.where.expr.lValue.getStringValue() == "id" &&
            statement.where.expr.op == parser::TokenType::ASSIGN) {
            res =
                storage.delete_data(statement.tableSource,
                                    statement.where.expr.rValue.getIntValue());
        } else {
        }
    } else {
    }
    if (res) {
        spdlog::info("delete successful");
    } else {
        spdlog::warn("delete failed");
    }
}

void SQLOperate::print_expr(const ast::SQLExprValue &v) {
    if (v.isInt()) {
        fmt::print("{:<10}", v.getIntValue());
    } else if (v.isFloat()) {
        fmt::print("{:<10}", v.getFloatValue());
    } else if (v.isString()) {
        fmt::print("{:<10}", v.getStringValue());
    }
}
} // namespace minidb::operate