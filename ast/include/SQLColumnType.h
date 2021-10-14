#pragma once

namespace minidb::ast {

class SQLColumnType {
    enum class Type { INT, FLOAT, CHAR, TEXT };
    Type columnType;
    int  length;
};
} // namespace minidb::ast