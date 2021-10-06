#include "Input.h"
#include "MinidbStrings.h"
#include "TokenType.h"
#include "Tokenizer.h"
#include <any>
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
    minidb::MinidbStrings::print_string(
        minidb::MinidbStrings::StringType::info);
    auto input = minidb::Input();
    input.readInputFromStream(std::cin);
    minidb::Tokenizer tokenizer(input.buffer);
    for (auto token = tokenizer.getNextToken();
         token.tokenType != minidb::TokenType::END &&
         token.tokenType != minidb::TokenType::ILLEGAL;
         token = tokenizer.getNextToken()) {
        std::cout << static_cast<std::underlying_type<minidb::TokenType>::type>(
                         token.tokenType)
                  << " " << token.val << "\n";
    }
    return 0;
}