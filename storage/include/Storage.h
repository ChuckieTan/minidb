#pragma once

#include "Pager.h"
#include "SQLBinaryData.h"
#include "SQLCreateTableStatement.h"
#include "SQLInsertIntoStatement.h"
#include "table_info.h"

#include <cstdint>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace minidb::storage {

struct FileMetaData {
    char          tag[ 7 ] = "Minidb";
    std::uint32_t table_num;
    std::uint32_t table_define_begin;
    std::uint32_t table_define_end;
};

struct TableDefineMetaData {
    std::uint32_t table_define_length;
    std::uint32_t table_root_addr;
    std::uint32_t first_leaf_addr;
    std::uint32_t last_leaf_addr;
    std::uint32_t column_num;
    std::uint32_t table_name_length;
};

class Storage {
public:
    Storage(const std::string &_fileName, bool _isInMemory);

    Pager pager;

    std::unordered_map<std::string, TableInfo> table_info_map;

    bool       scan_tables();
    TableInfo  scan_table(std::uint32_t &current_addr);
    ColumnInfo scan_column(std::uint32_t &current_addr);

    bool       new_table(const ast::SQLCreateTableStatement &creat_statement);
    ColumnInfo write_column_define(const ast::SQLColumnDefine &column_define,
                                   std::uint32_t &             current_addr);

    bool insert_data(const std::string &table_name, std::int32_t key,
                     SQLBinaryData data);

    SQLBinaryData search_data(const std::string &table_name, std::int32_t key);

    /**
     * @brief 写入指定大小的二进制数据，同时使current_addr往后移动
     */
    bool write_binary(const void *data, std::uint32_t size,
                      std::uint32_t &current_addr);

    std::uint32_t              table_num;
    static const std::uint32_t pageSize           = 4096;
    static const std::uint32_t tableNumAddr       = 7;
    std::uint32_t              table_define_begin = 19;
    std::uint32_t              table_define_end   = 19;
};
} // namespace minidb::storage