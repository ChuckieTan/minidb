#pragma once

#include "Pager.h"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace minidb::storage {

class BPlusTreeNode {
public:
    BPlusTreeNode(Pager &_pager);
    bool is_leaf() const;

    // static const int order = 253;
    static const int order = 4;

    std::uint64_t parent;
    std::uint64_t pre_leaf;
    std::uint64_t next_leaf;

    // len 为 keys 的长度
    // 对于叶子节点 value 的长度等于 len
    // 对于非叶子节点 children 的长度为 len + 1
    std::uint64_t              len;
    std::vector<std::int64_t>  keys;
    std::vector<std::uint64_t> children_or_value;

    bool _is_leaf;

    Pager &       pager;
    std::uint64_t addr;

    bool          can_add_entry() const;
    bool          insert_entry(std::int64_t key, std::uint64_t value);
    std::uint64_t get_entry(std::int64_t key);

    bool load(std::uint64_t addr);
    bool dump(std::uint64_t _addr = 0);
};
} // namespace minidb::storage