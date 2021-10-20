#pragma once

#include "Input.h"
#include "Lexer.h"
#include "Parser.h"
#include "Storage.h"
#include <string>
namespace minidb::operate {

class SQLOperate {
public:
    SQLOperate(std::string &&_file_name);
    void main_loop();

    util::Input      input;
    std::string      file_name;
    parser::Parser   parser;
    parser::Lexer    lexer;
    storage::Storage storage;
};
} // namespace minidb::operate