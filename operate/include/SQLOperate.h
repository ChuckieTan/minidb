#pragma once

#include "Input.h"
#include "Lexer.h"
#include "Parser.h"
#include "SQLExprValue.h"
#include "Storage.h"
#include <string>
namespace minidb::operate {

class SQLOperate {
public:
    SQLOperate(std::string &&_file_name);
    void main_loop();
    bool create_table_operate();
    bool insert_operate();
    bool select_operate();
    bool delete_operate();
    void print_expr(const ast::SQLExprValue &v);

    util::Input      input;
    std::string      file_name;
    parser::Parser   parser;
    parser::Lexer    lexer;
    storage::Storage storage;
};
} // namespace minidb::operate