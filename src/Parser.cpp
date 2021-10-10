#include "Parser.h"
#include <functional>
#include <initializer_list>
#include <iostream>
#include <string>
#include <variant>

namespace minidb {

Parser::MatchType::MatchType(const char *str) {
    _data    = str;
    dataType = DataType::STRING;
}

Parser::MatchType::MatchType(const std::string &str) {
    _data    = str;
    dataType = DataType::STRING;
}
Parser::MatchType::MatchType(const std::function<bool()> &func) {
    _data    = func;
    dataType = DataType::FUNC;
}
bool Parser::MatchType::isString() {
    return dataType == DataType::STRING;
}
bool Parser::MatchType::isFunc() {
    return dataType == DataType::FUNC;
}

std::string Parser::MatchType::getString() {
    if (isString()) {
        return std::get<std::string>(_data);
    } else {
        return "";
    }
}

std::function<bool()> Parser::MatchType::getFunc() {
    if (isFunc()) {
        return std::get<std::function<bool()>>(_data);
    } else {
        return []() { return false; };
    }
}

bool Parser::match(const std::function<bool()> &func) {
    return func();
}

bool Parser::match(const std::string &str) {
    return false;
}

bool Parser::chain(std::initializer_list<MatchType> matchCondition) {
    bool ans = true;
    for (auto m : matchCondition) {
        if (m.isString()) {
            ans = ans && match(m.getString());
        } else if (m.isFunc()) {
            ans = ans && match(m.getFunc());
        }
    }
    return ans;
}
} // namespace minidb