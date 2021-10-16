#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace minidb::storage {

class BPlusTreeNode {
    BPlusTreeNode();
    bool isLeaf() const;

    static const int           order = 512;
    BPlusTreeNode *            parent;
    std::vector<std::uint32_t> children;
};

class BPlusTreeLeafNode : public BPlusTreeNode {
public:
    BPlusTreeLeafNode();
};
} // namespace minidb::storage