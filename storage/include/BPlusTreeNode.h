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

    static const int order = 256;

    std::uint32_t              parent;
    std::uint32_t              len;
    std::vector<std::int32_t>  keys;
    std::vector<std::uint32_t> childrenOrValue;

    bool          _isLeaf;
    std::uint32_t nextLeaf;

    Pager &       pager;
    std::uint32_t addr;

    bool          can_add_entry() const;
    bool          insert_entry(std::int32_t key, std::uint32_t value);
    std::uint32_t get_entry(std::int32_t key);

    bool load(std::uint32_t addr);
    bool dump(std::uint32_t _addr = 0);
};
} // namespace minidb::storage