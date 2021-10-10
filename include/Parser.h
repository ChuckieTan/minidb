#pragma once

#include <functional>
#include <initializer_list>
#include <string>
#include <variant>

namespace minidb {

class Parser {
public:
    class MatchType {
    public:
        MatchType() = default;
        MatchType(const char *str);
        MatchType(const std::string &str);
        MatchType(const std::function<bool()> &func);

        bool isString();
        bool isFunc();

        std::string           getString();
        std::function<bool()> getFunc();

    private:
        enum class DataType { STRING, FUNC, INVALID };
        DataType dataType = DataType::INVALID;
        std::variant<std::string, std::function<bool()>> _data;
    };
    bool chain(std::initializer_list<MatchType> matchCondition);
    bool match(const std::string &str);
    bool match(const std::function<bool()> &func);
};
} // namespace minidb