#pragma once

#include "SQLColumnAssign.h"
#include "SQLWhereStatement.h"
#include <string>
#include <vector>

namespace minidb::ast {

class SQLUpdateStatement {
public:
    std::string                  tableSource;
    std::vector<SQLColumnAssign> columnAssign;
    SQLWhereStatement            where;
    bool                         isWhereExists;
};
} // namespace minidb::ast