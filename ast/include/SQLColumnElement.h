#pragma once

#include "SQLTableElement.h"
#include <string>

namespace minidb::ast {
class SQLColumnElement {
    SQLTableElement tableName;
    std::string columnName;
};
}