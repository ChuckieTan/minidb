#pragma once

#include "SQLWhereStatement.h"
#include <string>
#include <vector>

namespace minidb::ast {

class SQLSelectStatement {
public:
    std::vector<std::string> resultList;
    std::string              tableSource;
    SQLWhereStatement        where;
    bool                     isWhereExists;
};
} // namespace minidb::ast