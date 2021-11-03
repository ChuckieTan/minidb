#pragma once

#include "SQLColumnDefine.h"
#include <string>
#include <vector>

namespace minidb::ast {

class SQLCreateTableStatement {
public:
    std::string                  tableName;
    std::vector<SQLColumnDefine> columnDefineList;
};
} // namespace minidb::ast