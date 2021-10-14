#pragma once

#include "Lexer.h"
#include "TokenType.h"
#include <functional>
#include <string>
#include <variant>

namespace minidb::parser {

class Parser {
public:
    Parser(const Lexer &_lexer);
    Parser(std::string &&_sql);
    Parser(const std::string &_sql);
    class MatchType {
    public:
        MatchType() = default;
        MatchType(const std::string &str);
        MatchType(TokenType &&tokentype);
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
    bool match(MatchType condition);
    bool selectStatement();

protected:
    Lexer lexer;

    bool selectList();
    bool table();
    bool columnName();

    bool whereExpression();
    bool expression();
    bool expressionLValue();
    bool expressionRValue();

    bool compareOperator();
    bool logicalOperator();

    bool numbericLiteral();
    bool literalValue();
    
    bool identifier();
    bool functional();
    bool columnNameWithTable();
    bool resultColumn();
    bool chain(std::initializer_list<MatchType> args);
    bool many(std::initializer_list<MatchType> args);
    bool optional(std::initializer_list<MatchType> args);
    bool tree(std::initializer_list<MatchType> args);
};

} // namespace minidb