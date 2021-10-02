#include <iostream>
#include <string>
#include "Input.h"
#include "Strings.h"

int main(int argc, char *argv[]) {
    minidb::Strings::print_string(minidb::Strings::StringType::info);
    auto input = minidb::Input();
    input.readInputFromStream(std::cin);
    return 0;
}