#include "Input.h"
#include "Lexer.h"
#include "MinidbStrings.h"
#include "Parser.h"
#include "TokenType.h"
#include <iostream>
#include <spdlog/spdlog.h>
#include <string>

int main(int argc, char *argv[]) {
#ifdef DEBUG
    fmt::print("Debug Mode\n");
#endif
    using namespace minidb;
    util::MinidbStrings::print_string(
        util::MinidbStrings::StringType::info);
    auto input = util::Input();
    input.readInputFromStream(std::cin);
    parser::Lexer  lexer(input.buffer);
    parser::Parser parser(lexer);
    fmt::print("{}\n", parser.selectStatement());
    for (auto token = lexer.getNextToken();
         token.tokenType != parser::TokenType::END &&
         token.tokenType != parser::TokenType::ILLEGAL;
         token = lexer.getNextToken()) {
        fmt::print("{:<4} {}\n", token.tokenType, token.val);
    }
    return 0;
}