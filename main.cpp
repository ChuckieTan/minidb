#include "Input.h"
#include "MinidbStrings.h"
#include "Parser.h"
#include "TokenType.h"
#include "Lexer.h"
#include <any>
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
    minidb::MinidbStrings::print_string(
        minidb::MinidbStrings::StringType::info);
    auto input = minidb::Input();
    input.readInputFromStream(std::cin);
    minidb::Lexer lexer(input.buffer);
    minidb::Parser parser(lexer);
    std::cout << (parser.selectStatement()? 1: 0) << std::endl;
    for (auto token = lexer.getNextToken();
         token.tokenType != minidb::TokenType::END &&
         token.tokenType != minidb::TokenType::ILLEGAL;
         token = lexer.getNextToken()) {
        std::cout << static_cast<std::underlying_type<minidb::TokenType>::type>(
                         token.tokenType)
                  << " " << token.val << "\n";
    }
    return 0;
}