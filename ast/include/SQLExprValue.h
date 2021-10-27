#pragma once

#include <cstdint>
#include <string>
#include <variant>

namespace minidb::ast {
class SQLExprValue {
public:
    enum class DataType { INT, FLOAT, STRING, COLUMN };

    DataType                                        dataType;
    std::variant<std::int64_t, double, std::string> value;

    SQLExprValue() = default;
    explicit SQLExprValue(int _value);
    explicit SQLExprValue(std::int64_t _value);
    explicit SQLExprValue(double _value);
    explicit SQLExprValue(const std::string &_value);
    explicit SQLExprValue(const std::string &_value, DataType _dataType);

    bool isInt() const;
    bool isFloat() const;
    bool isString() const;
    bool isColumn() const;

    bool operator==(const SQLExprValue &v) const;

    std::int64_t getIntValue() const;
    double       getFloatValue() const;
    std::string  getStringValue() const;
    std::string  getColumnValue() const;
};
} // namespace minidb::ast