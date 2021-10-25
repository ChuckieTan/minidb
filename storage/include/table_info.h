#pragma once

#include "BPlusTree.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace minidb::storage {

struct ColumnInfo {
    std::string column_name;

    // 0: int, 1: float, 2: string
    std::uint8_t column_type;
};

struct TableInfo {
    std::string tableName;
    // 存储 root_addr 的地址，保存用于修改根节点
    std::uint32_t table_root_define_addr;
    // 存储 first_leaf_addr 的地址，保存用于修改第一个叶子节点
    std::uint32_t first_leaf_define_addr;
    // 存储 last_leaf_addr 的地址，保存用于修改最后一个叶子节点 
    std::uint32_t last_leaf_define_addr;

    // 根节点的地址
    std::uint32_t root_addr;
    // 第一个叶子节点的地址
    std::uint32_t first_leaf_addr;
    // 最后一个叶子节点的地址
    std::uint32_t last_leaf_addr;

    std::vector<ColumnInfo>    columns;
    std::shared_ptr<BPlusTree> b_plus_tree;
};
} // namespace minidb::storage