#pragma once

#include "Pager.h"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace minidb::storage {

class BPlusTreeNode {
public:
    BPlusTreeNode(Pager &_pager);
    bool isLeaf() const;

    static const int order = 254;

    std::uint64_t              parent;
    std::uint64_t              len;
    std::vector<std::int64_t>  keys;
    std::vector<std::uint64_t> childrenOrValue;

    bool          _isLeaf;

    std::uint64_t pre_leaf;
    std::uint64_t next_leaf;

    Pager &       pager;
    std::uint64_t addr;

    bool          can_add_entry() const;
    bool          insert_entry(std::int64_t key, std::uint64_t value);
    std::uint64_t get_entry(std::int64_t key);

    bool load(std::uint64_t addr);
    bool dump(std::uint64_t _addr = 0);
};
} // namespace minidb::storage