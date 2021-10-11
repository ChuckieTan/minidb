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
    minidb::MinidbStrings::print_string(
        minidb::MinidbStrings::StringType::info);
    auto input = minidb::Input();
    input.readInputFromStream(std::cin);
    minidb::Lexer  lexer(input.buffer);
    minidb::Parser parser(lexer);
    fmt::print("{}\n", parser.selectStatement());
    for (auto token = lexer.getNextToken();
         token.tokenType != minidb::TokenType::END &&
         token.tokenType != minidb::TokenType::ILLEGAL;
         token = lexer.getNextToken()) {
        fmt::print("{:<4} {}\n", token.tokenType, token.val);
    }
    return 0;
}