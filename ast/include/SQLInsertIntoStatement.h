#pragma once

#include <string>
#include <vector>

#include "ValuesClause.h"

namespace minidb::ast {

class SQLInsertIntoStatement {
public:
    std::string tableName;
    std::vector<std::string> cloumns;
    std::vector<ValuesClause> values;
};
}