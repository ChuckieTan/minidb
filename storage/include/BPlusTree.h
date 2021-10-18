#pragma once

#include <cstddef>
#include <cstdint>
#include <utility>

#include "BPlusTreeNode.h"
#include "Pager.h"
#include "SQLBinaryData.h"

namespace minidb::storage {

class BPlusTree {
public:
    BPlusTree(const std::string &_fileName, bool _isInMemory,
              std::uint32_t _root);
    ~BPlusTree();

    bool insert(std::int32_t key, char *data, std::size_t dataSize);
    bool remove(std::int32_t key);

    /**
     * @brief 返回 key 所对应的数据的字节流
     *
     * @param key
     * @return operate::SQLBinaryData key对应的字节流及其长度
     */
    operate::SQLBinaryData search(std::int32_t key);

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

    std::uint32_t split_leaf();
    std::uint32_t split_parent();

private:
    Pager          pager;
    BPlusTreeNode *currentNode;
    std::uint32_t  root;
};
} // namespace minidb::storage