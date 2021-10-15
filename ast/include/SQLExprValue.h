#pragma once

#include <variant>
#include <string>

namespace minidb::ast {
class SQLExprValue {
public:
    enum class DataType { INT, FLOAT, STRING, COLUMN };

    DataType                                                 dataType;
    std::variant<int, double, std::string> value;

    SQLExprValue() = default;
    SQLExprValue(int _value);
    SQLExprValue(double _value);
    SQLExprValue(const std::string &_value);
    SQLExprValue(const std::string &_value, DataType _dataType);

    bool isInt() const;
    bool isFloat() const;
    bool isString() const;
    bool isColumn() const;

    bool operator==(const SQLExprValue &v) const;

    int         getIntValue() const;
    double      getFloatValue() const;
    std::string getStringValue() const;
    std::string getColumnValue() const;
};
} // namespace minidb::ast