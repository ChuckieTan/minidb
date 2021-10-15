#pragma once

#include "SQLExprValue.h"
#include <string>

namespace minidb::ast {

class SQLColumnAssign {
public:
    std::string  columnName;
    SQLExprValue value;

    bool operator==(const SQLColumnAssign &assign) const;
};
} // namespace minidb::ast