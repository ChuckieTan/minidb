#pragma once

#include <string>

namespace minidb::ast {

class SQLTableElement {
    std::string tableName;
    std::string alias;
};
}