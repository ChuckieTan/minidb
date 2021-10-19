#pragma once

#include <cstddef>
#include <cstdint>
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
    BPlusTree(std::uint32_t _root, Pager &pager, Storage &_storage,
              const std::string &_table_name);
    ~BPlusTree();

    bool insert(std::int32_t key, SQLBinaryData);
    bool remove(std::int32_t key);

    /**
     * @brief 返回 key 所对应的数据的字节流
     *
     * @param key
     * @return storage::SQLBinaryData key对应的字节流及其长度
     */
    storage::SQLBinaryData search(std::int32_t key);

    /**
     * @brief 定位 currentNode 到 key 所应该在的节点
     *
     * @param key
     * @return true 定位成功
     * @return false 查找失败
     */
    bool search_in_tree(std::int32_t key);

    std::uint32_t    createNode();
    bool             changeRoot(std::uint32_t addr);
    static const int order = 256;

    bool split_node();

private:
    Pager &        pager;
    BPlusTreeNode *currentNode;
    std::uint32_t  root_addr;

    std::string table_name;
    Storage &   storage;
};
} // namespace minidb::storage