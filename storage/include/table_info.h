#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace minidb::storage {

struct ColumnInfo {
    std::string column_name;

    // 0: int, 1: float, 2: string
    std::uint8_t  column_type;
};

struct TableInfo {
    std::string             tableName;
    std::int32_t            root_addr;
    std::vector<ColumnInfo> columns;
};
} // namespace minidb::storage