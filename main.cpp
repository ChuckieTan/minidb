#include "Input.h"
#include "Lexer.h"
#include "MinidbStrings.h"
#include "Pager.h"
#include "Parser.h"
#include "SQLInsertIntoStatement.h"
#include "SQLOperate.h"
#include "Storage.h"
#include "TokenType.h"
#include "spdlog/fmt/bundled/core.h"
#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <iterator>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

int main(int argc, char *argv[]) {
#ifdef DEBUG
    fmt::print("Debug Mode\n");
#endif
    using namespace minidb;
    // util::MinidbStrings::print_string(
    //     util::MinidbStrings::StringType::info);
    // auto input = util::Input();
    // input.readInputFromStream(std::cin);
    // parser::Lexer  lexer(input.buffer);
    // parser::Parser parser(lexer);
    // // fmt::print("{}\n", parser.selectStatement());
    // for (auto token = lexer.getNextToken();
    //      token.tokenType != parser::TokenType::END &&
    //      token.tokenType != parser::TokenType::ILLEGAL;
    //      token = lexer.getNextToken()) {
    //     fmt::print("{:<4} {}\n", token.tokenType, token.val);
    // }

    // storage::Pager pager("student.db", false);
    // std::int64_t pos = pager.write("123", sizeof("123"));
    // std::int64_t size = pager.getSize(pos);
    // fmt::print("{} {}\n", pos, size);
    // std::vector<char> buffer(size);
    // pager.read(pos + sizeof(size), buffer.data(), size);
    // std::string str;
    // str.insert(str.begin(), buffer.begin(), buffer.end());
    // for(auto ch: buffer) {
    //     fmt::print("{}\n", ch);
    // }
    // std::cout << str << " " << "success\n";

    const std::array<const std::string, 5> sqls{
        "create table student (id int, name text);",
        "select * from student where id = 1;",
        "insert into student values (1, 'tom');",
        "update student set id = 1, name = 'tom' where id = 1;",
        "delete from student where id = 1;",
    };

    int v[ 1000 ];

    std::memset(v, 0, sizeof(v));

    for (int i = 0; i < 3000000; i++) {
        v[i%1000] +=
        minidb::parser::Parser(sqls[i%sqls.size()]).parseStatement();
    }
    fmt::print("{}\n", v[ 99 ]);
    // operate::SQLOperate operate("student.db");
    // operate.main_loop();

    return 0;
}