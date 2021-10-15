#pragma once

#include "SQLWhereStatement.h"
#include <string>

namespace minidb::ast {

class SQLDeleteStatement {
public:
    std::string       tableSource;
    SQLWhereStatement where;
    bool              isWhereExists;
};
} // namespace minidb::ast