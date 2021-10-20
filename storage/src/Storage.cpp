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

static char tag[ 7 ] = "Minidb";

// file data format:
// 'Minidb' | table_num | table_define_begin
// 7B       | 4B        | 4B
// table_define_end | (table_define)*
// 4B               | ...
//
// table define:
// table_define_length | table_name_length | table_name
// 4B                  | 4B                | ...
//       | table_root_addr | column_num | (column_define)*
//       | 4B              | 4B         | ...
//
// column_define:
// column_type | column_name_size | column_name
// 1B          | 4B               | ...
//
// column_type:
// 0: int(4B), 1: float(8B), 2: string(4B + ...)
Storage::Storage(const std::string &_fileName, bool _isInMemory)
    : pager(_fileName, _isInMemory) {
    auto file_size = pager.getFileSize();
    if (file_size == 0) {
        // 写入 4KB 作为 Metadata
        char *head = new char[ pageSize ]{ 0 };
        pager.write({ head, pageSize }, 0);
        delete[] head;

        // 写入数据库元信息
        std::uint32_t current_addr = 0;
        // 写入 Minidb 标识
        pager.write({ tag, sizeof(tag) }, current_addr);
        current_addr += sizeof(tag);

        table_num        = 0;
        table_define_end = table_define_begin;

        // 写入 tableNum
        pager.write({ (char *) &table_num, sizeof(table_num) }, current_addr);
    } else if (file_size >= 11) {
        std::uint32_t current_addr = 0;

        // 判断文件开头是否有 Minidb 标识
        char head[ 7 ];
        pager.read({ head, sizeof(head) }, current_addr);
        current_addr += sizeof(head);

        if (std::strcmp(head, tag) != 0) {
            spdlog::error("{} is not a Minidb database file", _fileName);
        }
        // 读入 table_num
        pager.read({ (char *) &table_num, sizeof(table_num) }, current_addr);
        current_addr += sizeof(table_num);

        // 读入 table_define_begin
        pager.read({ (char *) &table_define_begin, sizeof(table_define_begin) },
                   11);
        current_addr += sizeof(table_define_begin);

        // 读入 table_define_end
        pager.read({ (char *) &table_define_end, sizeof(table_define_end) },
                   15);
        current_addr += sizeof(table_define_end);

        // 扫描所有表的信息，并生成其对应的B+树
        scan_tables();

    } else {
        spdlog::error("{} is not a Minidb database file", _fileName);
    }
}

bool Storage::scan_tables() {
    // 循环获取所有表的表名和地址
    std::uint32_t current_addr = table_define_begin;
    for (std::uint32_t i = 0; i < table_num; i++) {
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

TableInfo Storage::scan_table(std::uint32_t &current_addr) {
    TableInfo table_info;

    // 获取表定义的长度
    std::uint32_t table_define_size;
    pager.read({ (char *) &table_define_size, sizeof(table_define_size) },
               current_addr);
    current_addr += sizeof(table_define_size);

    // 获取当前表名的大小
    std::uint32_t tableNameSize;
    pager.read({ (char *) &tableNameSize, sizeof(tableNameSize) },
               current_addr);
    current_addr += sizeof(tableNameSize);

    // 获取当前表名
    std::vector<char> tableNameSeq(tableNameSize);
    pager.read({ tableNameSeq.data(), tableNameSize }, current_addr);
    std::string tableName;
    tableName.insert(tableName.begin(), tableNameSeq.begin(),
                     tableNameSeq.end());
    current_addr += tableNameSize;

    // 获取存储表的根节点的地址
    table_info.table_root_define_addr = current_addr;
    // 获取表的首个节点地址
    std::uint32_t tableAddr;
    pager.read({ (char *) &tableAddr, sizeof(tableAddr) }, current_addr);
    current_addr += sizeof(tableAddr);

    // 获取列的数量
    std::uint32_t columnNum;
    pager.read({ (char *) &columnNum, sizeof(columnNum) }, current_addr);
    current_addr += sizeof(columnNum);

    // 获取所有列的信息
    ColumnInfo column_info;
    for (int i = 0; i < columnNum; i++) {
        table_info.columns.push_back(scan_column(current_addr));
    }

    table_info.tableName = tableName;
    table_info.root_addr = tableAddr;

    return table_info;
}

ColumnInfo Storage::scan_column(std::uint32_t &current_addr) {
    ColumnInfo column_info;

    // 获取列的数据类型
    std::uint8_t column_type;
    pager.read({ (char *) &column_type, sizeof(column_type) }, current_addr);
    current_addr += sizeof(column_type);

    // 获取列名长度
    std::uint32_t column_name_size;
    pager.read({ (char *) &column_name_size, sizeof(column_name_size) },
               current_addr);
    current_addr += sizeof(column_name_size);

    // 获取当前表名
    std::vector<char> column_name_seq(column_name_size);
    pager.read({ column_name_seq.data(), column_name_size }, current_addr);
    std::string column_name;
    column_name.insert(column_name.begin(), column_name_seq.begin(),
                       column_name_seq.end());
    current_addr += column_name_size;

    column_info.column_name = column_name;
    column_info.column_type = column_type;
    return column_info;
}

bool Storage::write_binary(const void *data, std::uint32_t size,
                           std::uint32_t &current_addr) {
    pager.write({ (char *) data, size }, current_addr);
    current_addr += size;
    return true;
}

bool Storage::new_table(const ast::SQLCreateTableStatement &creat_statement) {
    TableInfo table_info;

    // 先不写 table define 的长度
    std::uint32_t current_addr = table_define_end + 4;

    // 写入 table name 的长度
    auto &        table_name = creat_statement.tableName;
    std::uint32_t size       = table_name.size() + 1;
    write_binary(&size, sizeof(size), current_addr);

    // 写入 table name
    write_binary(table_name.c_str(), table_name.size() + 1, current_addr);

    // 写入存储root_addr的地址
    table_info.table_root_define_addr = current_addr;

    // 写入 table root 节点地址
    std::uint32_t table_root_addr = 0;
    write_binary(&table_root_addr, sizeof(table_root_addr), current_addr);

    // 写入列的数量
    std::uint32_t column_num = creat_statement.columnDefineList.size();
    write_binary(&column_num, sizeof(column_num), current_addr);

    // 写入所有列，并且放入当前table列表中
    for (std::uint32_t i = 0; i < column_num; i++) {
        auto column_info = write_column_define(
            creat_statement.columnDefineList[ i ], current_addr);
        table_info.columns.push_back(column_info);
    }

    // 写入 table define 的长度
    std::uint32_t table_define_length = current_addr - table_define_end;
    write_binary(&table_define_length, sizeof(table_define_length),
                 table_define_end);
    table_define_end = current_addr;

    // 更新 table_define_end
    write_binary(&table_define_end, sizeof(table_define_end), current_addr);

    table_info.root_addr         = table_root_addr;
    table_info.tableName         = table_name;
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
                                 std::uint32_t &             current_addr) {
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
    std::uint32_t column_name_size = column_define.columnName.size() + 1;
    write_binary(&column_name_size, sizeof(column_name_size), current_addr);

    // 写入列名
    write_binary(column_define.columnName.c_str(), column_name_size,
                 current_addr);

    ColumnInfo column_info;
    column_info.column_name = column_define.columnName;
    column_info.column_type = column_type;
    return column_info;
}

bool Storage::insert_data(const std::string &table_name, std::int32_t key,
                          SQLBinaryData data) {
    auto table_info = table_info_map[ table_name ];
    table_info.b_plus_tree->insert(key, data);
    return true;
}

SQLBinaryData Storage::search_data(const std::string &table_name, std::int32_t key) {
    auto table_info = table_info_map[ table_name ];
    return table_info.b_plus_tree->search(key);
}

} // namespace minidb::storage