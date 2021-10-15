#pragma once

#include <string>
#include <vector>

#include "SQLExprValue.h"

namespace minidb::ast {

class SQLInsertIntoStatement {
public:
    std::string tableName;
    std::vector<std::string> cloumns;
    std::vector<SQLExprValue> values;
};
}