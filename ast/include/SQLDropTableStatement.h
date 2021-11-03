#pragma once

#include <string>

namespace minidb::ast {

class SQLDropTableStatement {
public:
    bool        ifExists;
    std::string tableName;
};
} // namespace minidb::ast