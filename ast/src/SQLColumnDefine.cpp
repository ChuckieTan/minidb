#include "SQLColumnDefine.h"

namespace minidb::ast {

std::unordered_map<std::string, SQLColumnDefine::ColumnType>
    SQLColumnDefine::typeMap = { { "int", ColumnType::INT },
                                 { "float", ColumnType::FLOAT },
                                 { "text", ColumnType::TEXT } };

SQLColumnDefine::SQLColumnDefine(const std::string &_columnName,
                                 ColumnType         _columnType)
    : columnName(_columnName)
    , columnType(_columnType) {}
bool SQLColumnDefine::operator==(const SQLColumnDefine &define) const {
    return columnName == define.columnName && columnType == define.columnType;
}

bool SQLColumnDefine::is_int() const { return columnType == ColumnType::INT; }

bool SQLColumnDefine::is_float() const {
    return columnType == ColumnType::FLOAT;
}

bool SQLColumnDefine::is_text() const { return columnType == ColumnType::TEXT; }

SQLColumnDefine::SQLColumnDefine(std::string &&_columnName,
                                 ColumnType    _columnType)
    : columnName(_columnName)
    , columnType(_columnType) {}
} // namespace minidb::ast