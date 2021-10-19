#pragma once

#include "Pager.h"
#include "SQLCreateTableStatement.h"
#include "table_info.h"
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace minidb::storage {

class Storage {
public:
    Storage(const std::string &_fileName, bool _isInMemory);

    Pager                  pager;
    std::vector<TableInfo> table_info_list;

    bool       scan_tables();
    TableInfo  scan_table(std::uint32_t &current_addr);
    ColumnInfo scan_column(std::uint32_t &current_addr);

    bool new_table(const ast::SQLCreateTableStatement &creat_statement);
    bool write_column_define(const ast::SQLColumnDefine &column_define,
                             std::uint32_t &             current_addr);
    /**
     * @brief 写入指定大小的二进制数据，同时使current_addr往后移动
     */
    bool write_binary(const void *data, std::uint32_t size,
                      std::uint32_t &current_addr);

    std::uint32_t              table_num;
    static const std::uint32_t pageSize           = 4096;
    static const std::uint32_t tableNumAddr       = 7;
    std::uint32_t              table_define_begin = 11;
    std::uint32_t              table_define_end   = 0;
};
} // namespace minidb::storage