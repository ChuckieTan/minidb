#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>

#include "BPlusTreeNode.h"
#include "Pager.h"
#include "SQLBinaryData.h"

namespace minidb::storage {

// Storage 前置声明， 解决循环依赖
class Storage;

class BPlusTree {
public:
    BPlusTree(std::uint64_t _root, Pager &pager, Storage &_storage,
              const std::string &_table_name);

    bool insert(std::int64_t key, const SQLBinaryData &data);
    bool remove(std::int64_t key);

    /**
     * @brief 返回 key 所对应的数据的字节流
     *
     * @param key
     * @return storage::SQLBinaryData key对应的字节流及其长度
     */
    storage::SQLBinaryData search(std::int64_t key);

    /**
     * @brief 定位 currentNode 到 key 所应该在的节点
     *
     * @param key
     * @return true 定位成功
     * @return false 查找失败
     */
    bool search_in_tree(std::int64_t key);

    std::uint64_t    createNode();
    bool             change_root(std::uint64_t addr);
    bool             change_first_leaf(std::uint64_t addr);
    bool             change_last_leaf(std::uint64_t addr);
    static const int order = 254;

    bool split_leaf();
    bool split_parent();

private:
    std::shared_ptr<BPlusTreeNode> currentNode;

    std::uint64_t root_addr;
    std::string   table_name;
    Pager &       pager;
    Storage &     storage;

    std::uint64_t first_leaf_addr;
    std::uint64_t last_leaf_addr;
};
} // namespace minidb::storage