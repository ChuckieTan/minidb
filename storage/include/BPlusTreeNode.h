#pragma once

#include "Pager.h"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace minidb::storage {

class BPlusTreeNode {
public:
    BPlusTreeNode();
    bool isLeaf() const;

    static const int order = 256;

    std::uint32_t              parent;
    std::uint32_t              len;
    std::vector<std::int32_t>  keys;
    std::vector<std::uint32_t> childrenOrValue;

    bool          _isLeaf;
    std::uint32_t nextLeaf;

    bool loads(std::uint32_t addr, Pager &pager);
};
} // namespace minidb::storage