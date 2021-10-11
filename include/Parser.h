#pragma once

#include "Lexer.h"
#include "TokenType.h"
#include <functional>
#include <string>
#include <variant>

namespace minidb {

class Parser {
public:
    Parser(const Lexer &_lexer);
    Parser(std::string &&_sql);
    Parser(const std::string &_sql);
    class MatchType {
    public:
        MatchType() = default;
        MatchType(const std::string &str);
        MatchType(const TokenType tokenType);
        MatchType(const std::function<bool()> &func);

        bool isString();
        bool isFunc();
        bool isToken();

        std::string           getString();
        TokenType             getToken();
        std::function<bool()> getFunc();

    private:
        enum class DataType { STRING, TOKEN, FUNC, INVALID };
        DataType dataType = DataType::INVALID;
        std::variant<std::string, TokenType, std::function<bool()>> _data;
    };
    bool match(MatchType &condition);
    bool selectStatement();
    bool whereStatement();

protected:
    Lexer lexer;

    bool selectList();
    bool table();
    bool columnName();
    bool identifier();
    bool functional();
    bool field();
    bool chain(std::initializer_list<MatchType> args);
    bool many(std::initializer_list<MatchType> args);
    bool tree(std::initializer_list<MatchType> args);
};

} // namespace minidb