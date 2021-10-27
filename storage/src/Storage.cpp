#include "Storage.h"
#include "BPlusTree.h"
#include "Pager.h"
#include "SQLBinaryData.h"
#include "SQLColumnDefine.h"
#include "spdlog/spdlog.h"
#include "table_info.h"
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>

namespace minidb::storage {

static char index_tag[ 13 ] = "Minidb Index";
static char data_tag[ 12 ] = "Minidb Data";

// struct FileMetaData {
//     char          tag[ 7 ] = "Minidb";
//     std::uint64_t table_num;
//     std::uint64_t table_define_begin;
//     std::uint64_t table_define_end;
// };

// struct TableDefineMetaData {
//     std::uint64_t table_root_addr;
//     std::uint64_t first_leaf_addr;
//     std::uint64_t last_leaf_addr;
//     std::uint64_t column_num;
// };

// file data format:
// 'Minidb' | table_num | table_define_begin
// 7B       | 4B        | 4B
// table_define_end | (table_define)*
// 4B               | ...
//
// table name 放在最后，前面的由结构体一块写入
// table define:
// table_define_length | table_root_addr | first_leaf_addr
//       | last_leaf_addr | column_num | table_name_length
//       | table_name| (column_define)*
//
// column_define:
// column_type | column_name_size | column_name
// 1B          | 4B               | ...
//
// column_type:
// 0: int(4B), 1: float(8B), 2: string(4B + ...)
Storage::Storage(const std::string &_fileName, bool _isInMemory)
    : pager(_fileName, _isInMemory) {
    auto file_size = pager.get_index_file_size();
    if (file_size == 0) {
        // 写入 4KB 作为 Metadata
        char *head = new char[ pageSize ]{ 0 };
        pager.write_index_file(head, pageSize, 0);
        delete[] head;

        IndexFileMetaData index_file_meta_data;
        index_file_meta_data.table_num          = 0;
        index_file_meta_data.table_define_begin = sizeof(index_file_meta_data);
        index_file_meta_data.table_define_end   = sizeof(index_file_meta_data);

        table_num          = index_file_meta_data.table_num;
        table_define_begin = index_file_meta_data.table_define_begin;
        table_define_end   = index_file_meta_data.table_define_end;

        // 写入数据库元信息
        std::uint64_t current_addr = 0;
        write_binary(&index_file_meta_data, sizeof(index_file_meta_data),
                     current_addr);
        
        DataFileMetaData data_file_meta_data;
        pager.write_data_file(&data_file_meta_data, sizeof(data_file_meta_data), 0);
    } else if (file_size >= 4096) {
        std::uint64_t current_addr = 0;

        // 读取元信息
        IndexFileMetaData index_file_meta_data;
        pager.read_index_file(&index_file_meta_data,
                              sizeof(index_file_meta_data), current_addr);
        current_addr += sizeof(index_file_meta_data);

        // 判断 Index 文件开头是否有 Minidb 标识
        if (std::strcmp(index_file_meta_data.tag, index_tag) != 0) {
            spdlog::error("{} is not a Minidb database file", _fileName);
            throw "not a minidb database file";
        }

        DataFileMetaData data_file_meta_data;

        pager.read_data_file(&data_file_meta_data,
                              sizeof(data_file_meta_data), 0);
        // 判断文件开头是否有 Minidb 标识
        if (std::strcmp(data_file_meta_data.tag, data_tag) != 0) {
            spdlog::error("{} is not a Minidb database file", _fileName);
            throw "not a minidb database file";
        }

        table_num          = index_file_meta_data.table_num;
        table_define_begin = index_file_meta_data.table_define_begin;
        table_define_end   = index_file_meta_data.table_define_end;

        // 扫描所有表的信息，并生成其对应的B+树
        scan_tables();
    } else {
        spdlog::error("{} is not a Minidb database file", _fileName);
    }
}

bool Storage::scan_tables() {
    // 循环获取所有表的表名和地址
    std::uint64_t current_addr = table_define_begin;
    for (std::uint64_t i = 0; i < table_num; i++) {
        TableInfo table_info = scan_table(current_addr);
        spdlog::info("scan table {}", table_info.tableName);
        table_info_map[ table_info.tableName ] = table_info;

        // 生成B+树
        // 只能在添加到 table_info_map 后才能生成，
        // 因为生成新树需要修改 table_info_map
        table_info_map[ table_info.tableName ].b_plus_tree =
            std::make_shared<BPlusTree>(table_info.root_addr, pager, *this,
                                        table_info.tableName);
    }
    table_define_end = current_addr;
    return true;
}

TableInfo Storage::scan_table(std::uint64_t &current_addr) {
    // 读取 table_define_meta_data
    TableDefineMetaData table_define_meta_data;
    pager.read_index_file(&table_define_meta_data,
                          sizeof(table_define_meta_data), current_addr);
    current_addr += sizeof(table_define_meta_data);

    TableInfo table_info;

    std::uint64_t table_name_length = table_define_meta_data.table_name_length;
    // 获取当前表名
    std::vector<char> table_name_seq(table_name_length);
    pager.read_index_file(table_name_seq.data(), table_name_length,
                          current_addr);
    std::string table_name;
    table_name.insert(table_name.begin(), table_name_seq.begin(),
                      table_name_seq.end());
    current_addr += table_name_length;

    // 获取所有列的信息
    ColumnInfo column_info;
    for (int i = 0; i < table_define_meta_data.column_num; i++) {
        table_info.columns.push_back(scan_column(current_addr));
    }

    table_info.tableName       = table_name;
    table_info.root_addr       = table_define_meta_data.table_root_addr;
    table_info.first_leaf_addr = table_define_meta_data.first_leaf_addr;
    table_info.last_leaf_addr  = table_define_meta_data.last_leaf_addr;
    return table_info;
}

ColumnInfo Storage::scan_column(std::uint64_t &current_addr) {
    ColumnInfo column_info;

    // 获取列的数据类型
    std::uint8_t column_type;
    pager.read_index_file(&column_type, sizeof(column_type), current_addr);
    current_addr += sizeof(column_type);

    // 获取列名长度
    std::uint64_t column_name_size;
    pager.read_index_file(&column_name_size, sizeof(column_name_size),
                          current_addr);
    current_addr += sizeof(column_name_size);

    // 获取当前表名
    std::vector<char> column_name_seq(column_name_size);
    pager.read_index_file(column_name_seq.data(), column_name_size,
                          current_addr);
    std::string column_name;
    column_name.insert(column_name.begin(), column_name_seq.begin(),
                       column_name_seq.end());
    current_addr += column_name_size;

    column_info.column_name = column_name;
    column_info.column_type = column_type;
    return column_info;
}

bool Storage::write_binary(const void *data, std::uint64_t size,
                           std::uint64_t &current_addr) {
    pager.write_index_file(data, size, current_addr);
    current_addr += size;
    return true;
}

bool Storage::new_table(const ast::SQLCreateTableStatement &create_statement) {
    auto &table_name = create_statement.tableName;

    TableDefineMetaData table_define_meta_data;
    // 写入table定义信息
    // table define,table_root_addr,first_leaf_addr,
    // last_leaf_addr 初始为0，后面更改
    table_define_meta_data.table_define_length = 0;
    table_define_meta_data.table_root_addr     = 0;
    table_define_meta_data.first_leaf_addr     = 0;
    table_define_meta_data.last_leaf_addr      = 0;

    table_define_meta_data.column_num =
        create_statement.columnDefineList.size();
    table_define_meta_data.table_name_length = table_name.size();

    TableInfo table_info;

    table_info.tableName = table_name;

    table_info.root_addr = 0;

    table_info.table_root_define_addr =
        table_define_end + (char *) &table_define_meta_data.table_root_addr -
        (char *) &table_define_meta_data;

    table_info.first_leaf_define_addr =
        table_define_end + (char *) &table_define_meta_data.first_leaf_addr -
        (char *) &table_define_meta_data;

    table_info.last_leaf_define_addr =
        table_define_end + (char *) &table_define_meta_data.last_leaf_addr -
        (char *) &table_define_meta_data;

    std::uint64_t current_addr = table_define_end;

    // 写入 table 定义信息
    write_binary(&table_define_meta_data, sizeof(table_define_meta_data),
                 current_addr);

    // 写入 table name
    write_binary(table_name.c_str(), table_name.size(), current_addr);

    // 写入所有列，并且放入当前table列表中
    for (const auto &column_define : create_statement.columnDefineList) {
        auto column_info = write_column_define(column_define, current_addr);
        table_info.columns.push_back(column_info);
    }

    // 写入 table define 的长度
    std::uint64_t table_define_length = current_addr - table_define_end;
    write_binary(&table_define_length, sizeof(table_define_length),
                 table_define_end);

    IndexFileMetaData file_meta_data;

    // 更新 table_define_end
    table_define_end = current_addr;
    // 更新磁盘上的 table_define_end
    std::uint64_t table_define_end_addr =
        (char *) &file_meta_data.table_define_end - (char *) &file_meta_data;
    write_binary(&table_define_end, sizeof(table_define_end),
                 table_define_end_addr);

    // 更新 table_num
    table_num += 1;
    std::uint64_t table_num_addr =
        (char *) &file_meta_data.table_num - (char *) &file_meta_data;
    write_binary(&table_num, sizeof(table_num), table_num_addr);

    table_info_map[ table_name ] = table_info;

    // 生成B+树
    // 只能在添加到 table_info_map 后才能生成，
    // 因为生成新树需要修改 table_info_map
    table_info_map[ table_name ].b_plus_tree = std::make_shared<BPlusTree>(
        table_info.root_addr, pager, *this, table_info.tableName);

    spdlog::info("create a new table: {}", table_name);
    return true;
}

ColumnInfo
    Storage::write_column_define(const ast::SQLColumnDefine &column_define,
                                 std::uint64_t &             current_addr) {
    // 写入 column type
    std::uint8_t column_type = 0;
    if (column_define.is_int()) {
        column_type = 0;
    } else if (column_define.is_float()) {
        column_type = 1;
    } else if (column_define.is_text()) {
        column_type = 2;
    }
    write_binary(&column_type, sizeof(column_type), current_addr);

    // 写入列名字符串长度，加上 '\0'
    std::uint64_t column_name_size = column_define.columnName.size();
    write_binary(&column_name_size, sizeof(column_name_size), current_addr);

    // 写入列名
    write_binary(column_define.columnName.c_str(), column_name_size,
                 current_addr);

    ColumnInfo column_info;
    column_info.column_name = column_define.columnName;
    column_info.column_type = column_type;
    return column_info;
}

bool Storage::insert_data(const std::string &table_name, std::int64_t key,
                          const SQLBinaryData &data) {
    auto table_info = table_info_map[ table_name ];
    auto res        = table_info.b_plus_tree->insert(key, data);
    return res;
}

SQLBinaryData Storage::search_data(const std::string &table_name,
                                   std::int64_t       key) {
    auto table_info = table_info_map[ table_name ];
    return table_info.b_plus_tree->search(key);
}

} // namespace minidb::storage