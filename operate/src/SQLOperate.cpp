#include "SQLOperate.h"
#include "BPlusTree.h"
#include "BinaryOperate.h"
#include "Lexer.h"
#include "Parser.h"
#include "TokenType.h"
#include "spdlog/spdlog.h"
#include <iostream>

namespace minidb::operate {

SQLOperate::SQLOperate(std::string &&_file_name)
    : file_name(_file_name)
    , parser("")
    , lexer("")
    , storage(_file_name, false) {}

void SQLOperate::main_loop() {
    input.readInputFromStream(std::cin);
    while (input.buffer != ".quit;") {
        lexer  = parser::Lexer(input.buffer);
        parser = parser::Parser(lexer);
        spdlog::info("{}", lexer.getCurrentToken().val);
        if (lexer.getCurrentToken().tokenType == parser::TokenType::CREATE) {
            auto statement = parser.parseCreateTableStatement();
            spdlog::info("{}", statement.tableName);
            storage.new_table(statement);
        } else if (lexer.getCurrentToken().tokenType ==
                   parser::TokenType::INSERT) {
            auto statement = parser.parseInsertIntoStatement();
            auto data      = BinaryOperate::dump(statement.values);
            storage.insert_data(statement.tableName,
                                statement.values[ 0 ].getIntValue(), data);
        } else if (lexer.getCurrentToken().tokenType ==
                   parser::TokenType::SELECT) {
            auto statement = parser.parseSelectStatement();
            auto data      = storage.search_data(statement.tableSource, 1);
            auto expr      = BinaryOperate::load(data);
            spdlog::info("{} {} {}", expr.size(), expr[ 0 ].getIntValue(),
                         expr[ 1 ].getStringValue());
        }
        input.readInputFromStream(std::cin);
    }
}
} // namespace minidb::operate