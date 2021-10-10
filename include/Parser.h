#pragma once

#include "Lexer.h"
#include <functional>
#include <string>

namespace minidb {

class Parser {
public:
    Lexer lexer;
    // chain可变参数模板的递归末尾
    template <typename MatchCondition>
    bool chain(const MatchCondition &condition);

    // 可变参数模板，实现 chain(arg1, arg2, ...)
    template <typename FirstCondition, typename... RestCondition>
    bool chain(const FirstCondition &firstCondition,
               const RestCondition &... restCondition);
    
    bool match(const std::string &str);
    bool match(const std::function<bool()> &func);
};

// 可变参数模板的实现
template <typename MatchCondition>
bool Parser::chain(const MatchCondition &condition) {
    return match(condition);
}

template <typename FirstCondition, typename... RestCondition>
bool Parser::chain(const FirstCondition &firstCondition,
                   const RestCondition &... restCondition) {
    return match(firstCondition) && chain(restCondition...);
}
} // namespace minidb