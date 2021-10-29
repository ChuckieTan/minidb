#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "BPlusTreeNode.h"
#include "Pager.h"
#include "SQLBinaryData.h"

namespace minidb::storage {

// Storage 前置声明， 解决循环依赖
class Storage;

class BPlusTree {
public:
    BPlusTree(std::uint64_t _root, std::uint64_t _first_leaf_addr,
              std::uint64_t _last_leaf_addr, Pager &_pager, Storage &_storage,
              const std::string &_table_name);

    // 定位 currentNode 到 key 所应该在的节点
    bool search_in_tree(std::int64_t key);
    // 返回 key 所对应的数据的字节流
    storage::SQLBinaryData search(std::int64_t key);
    bool                   insert(std::int64_t key, const SQLBinaryData &data);
    bool                   remove(std::int64_t key);

    bool             change_root(std::uint64_t addr);
    bool             change_first_leaf(std::uint64_t addr);
    bool             change_last_leaf(std::uint64_t addr);
    static const int order = 4;
    // static const int order = 253;

    bool split_leaf();
    bool split_parent();

    bool merge_leaf();

    class BPlusTreeIterator {
    public:
        BPlusTreeIterator(Pager &pager);
        BPlusTreeIterator(const BPlusTreeIterator &it);

        SQLBinaryData      operator*();
        bool               operator!=(const BPlusTreeIterator &that);
        BPlusTreeIterator &operator++();

        std::uint64_t                  current_record_offset;
        std::shared_ptr<BPlusTreeNode> current_node;
    };
    using iterator = BPlusTreeIterator;

    iterator  begin();
    iterator &end();

private:
    std::shared_ptr<BPlusTreeNode> current_node;

    std::uint64_t root_addr;
    std::string   table_name;
    Pager &       pager;
    Storage &     storage;

    std::uint64_t first_leaf_addr;
    std::uint64_t last_leaf_addr;

    iterator _end;
};
} // namespace minidb::storage