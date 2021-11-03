#include "SQLExprValue.h"
#include <cstdint>
#include <string>

namespace minidb::ast {

SQLExprValue::SQLExprValue(int _value)
    : value((std::int64_t) _value) {
    dataType = DataType::INT;
}

SQLExprValue::SQLExprValue(std::int64_t _value)
    : value(_value) {
    dataType = DataType::INT;
}

SQLExprValue::SQLExprValue(double _value)
    : value(_value) {
    dataType = DataType::FLOAT;
}

SQLExprValue::SQLExprValue(const std::string &_value)
    : value(_value) {
    dataType = DataType::STRING;
}
SQLExprValue::SQLExprValue(const std::string &_value, DataType _dataType)
    : value(_value)
    , dataType(_dataType) {}

bool SQLExprValue::isInt() const { return dataType == DataType::INT; }
bool SQLExprValue::isFloat() const { return dataType == DataType::FLOAT; }
bool SQLExprValue::isString() const { return dataType == DataType::STRING; }
bool SQLExprValue::isColumn() const { return dataType == DataType::COLUMN; }

bool SQLExprValue::operator==(const SQLExprValue &v) const {
    if (dataType == v.dataType) {
        if (isInt()) {
            return getIntValue() == v.getIntValue();
        } else if (isFloat()) {
            return getFloatValue() == v.getFloatValue();
        } else if (isString()) {
            return getStringValue() == v.getStringValue();
        } else if (isColumn()) {
            return getColumnValue() == v.getColumnValue();
        }
    } else {
        return false;
    }
    return false;
}

std::int64_t SQLExprValue::getIntValue() const {
    return std::get<std::int64_t>(value);
}

double SQLExprValue::getFloatValue() const { return std::get<double>(value); }

std::string SQLExprValue::getStringValue() const {
    return std::get<std::string>(value);
}

std::string SQLExprValue::getColumnValue() const {
    return std::get<std::string>(value);
}
} // namespace minidb::ast