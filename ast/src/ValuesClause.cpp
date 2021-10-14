#include "ValuesClause.h"
#include <string>

namespace minidb::ast {

ValuesClause::ValuesClause(int _value)
    : value(_value) {
    dataType = DataType::INT;
}

ValuesClause::ValuesClause(double _value)
    : value(_value) {
    dataType = DataType::FLOAT;
}

ValuesClause::ValuesClause(const std::string &_value)
    : value(_value) {
    dataType = DataType::TEXT;
}

bool ValuesClause::isInt() const { return dataType == DataType::INT; }
bool ValuesClause::isFloat() const { return dataType == DataType::FLOAT; }
bool ValuesClause::isText() const { return dataType == DataType::TEXT; }

bool ValuesClause::operator==(const ValuesClause &v) const {
    if (dataType == v.dataType) {
        if (isInt()) {
            return getIntValue() == v.getIntValue();
        } else if (isFloat()) {
            return getFloatValue() == v.getFloatValue();
        } else if (isText()) {
            return getTextValue() == v.getTextValue();
        }
    } else {
        return false;
    }
    return false;
}

int    ValuesClause::getIntValue() const { return std::get<int>(value); }
double ValuesClause::getFloatValue() const { return std::get<double>(value); }
std::string ValuesClause::getTextValue() const {
    return std::get<std::string>(value);
}
} // namespace minidb::ast