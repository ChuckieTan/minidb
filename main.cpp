#include "Input.h"
#include "Lexer.h"
#include "MinidbStrings.h"
#include "Pager.h"
#include "Parser.h"
#include "SQLOperate.h"
#include "Storage.h"
#include "TokenType.h"
#include "spdlog/fmt/bundled/core.h"
#include <c++/10/bits/c++config.h>
#include <cstdint>
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
    // std::int32_t pos = pager.write("123", sizeof("123"));
    // std::int32_t size = pager.getSize(pos);
    // fmt::print("{} {}\n", pos, size);
    // std::vector<char> buffer(size);
    // pager.read(pos + sizeof(size), buffer.data(), size);
    // std::string str;
    // str.insert(str.begin(), buffer.begin(), buffer.end());
    // for(auto ch: buffer) {
    //     fmt::print("{}\n", ch);
    // }
    // std::cout << str << " " << "success\n";

    operate::SQLOperate operate("student.db");
    operate.main_loop();
    
    return 0;
}