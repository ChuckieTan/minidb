#pragma once

#include <string>
#include <unordered_map>
namespace minidb::ast {

class SQLColumnDefine {
public:
    enum class ColumnType { INT, FLOAT, TEXT };
    ColumnType                                         columnType;
    std::string                                        columnName;
    static std::unordered_map<std::string, ColumnType> typeMap;

    SQLColumnDefine() {}
    SQLColumnDefine(const std::string &_columnName, ColumnType _columnType);
    SQLColumnDefine(std::string &&_columnName, ColumnType _columnType);

    bool operator==(const SQLColumnDefine &define) const;
    void setColumnType(const std::string &str) { columnType = typeMap[ str ]; }
};
} // namespace minidb::ast