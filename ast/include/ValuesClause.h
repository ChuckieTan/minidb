#pragma once

#include <string>
#include <variant>
namespace minidb::ast {

class ValuesClause {
public:
    enum class DataType { INT, FLOAT, TEXT };
    DataType                               dataType;
    std::variant<int, double, std::string> value;

    ValuesClause() = default;
    ValuesClause(int _value);
    ValuesClause(double _value);
    ValuesClause(const std::string &_value);

    bool isInt() const;
    bool isFloat() const;
    bool isText() const;

    bool operator==(const ValuesClause &v) const;

    int         getIntValue() const;
    double      getFloatValue() const;
    std::string getTextValue() const;
};
} // namespace minidb::ast