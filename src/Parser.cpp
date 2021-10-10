#include "Parser.h"
#include <iostream>
#include <string>

namespace minidb {

bool Parser::match(const std::function<bool()> &func) {
    return func();
}

bool Parser::match(const std::string &str) {
    return false;
}

} // namespace minidb