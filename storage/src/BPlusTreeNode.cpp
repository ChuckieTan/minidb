#include "BPlusTreeNode.h"
#include "Pager.h"

#include <algorithm>
#include <spdlog/spdlog.h>

namespace minidb::storage {

BPlusTreeNode::BPlusTreeNode(Pager &_pager)
    : parent(0)
    , keys(order)
    , childrenOrValue(order)
    , _isLeaf(true)
    , pre_leaf(0)
    , next_leaf(0)
    , len(0)
    , pager(_pager) {}

bool BPlusTreeNode::load(std::uint32_t _addr) {
    std::uint32_t current_addr = _addr;

    // 读入父节点地址
    pager.read(&(parent), sizeof(parent), current_addr);
    current_addr += sizeof(parent);

    // 读入元素数量
    pager.read(&(len), sizeof(len), current_addr);
    current_addr += sizeof(len);

    // 读入 key 列表
    pager.read(keys.data(), sizeof(keys[ 0 ]) * order, current_addr);
    current_addr += sizeof(keys[ 0 ]) * order;

    // 读入 childrenOrValue 列表
    pager.read(childrenOrValue.data(), sizeof(childrenOrValue[ 0 ]) * order,
               current_addr);
    current_addr += sizeof(childrenOrValue[ 0 ]) * order;

    // 读入 _isLeaf
    pager.read(&(_isLeaf), sizeof(_isLeaf), current_addr);
    current_addr += sizeof(_isLeaf);

    // 读入 pre_leaf
    pager.read(&(pre_leaf), sizeof(pre_leaf), current_addr);
    current_addr += sizeof(pre_leaf);

    // 读入 next_leaf
    pager.read(&(next_leaf), sizeof(next_leaf), current_addr);
    current_addr += sizeof(next_leaf);

    addr = _addr;
    return true;
}

bool BPlusTreeNode::dump(std::uint32_t _addr) {
    std::uint32_t current_addr;
    if (_addr == 0) {
        // 保存到当前所存的位置
        current_addr = addr;
    } else {
        // 保存到新位置
        current_addr = _addr;
        addr         = _addr;
    }

    // 写入父节点地址
    pager.write(&(parent), sizeof(parent), current_addr);
    current_addr += sizeof(parent);

    // 写入元素数量
    pager.write(&(len), sizeof(len), current_addr
                // namespace minidb::storage
    );
    current_addr += sizeof(len);

    // 写入 key 列表
    pager.write(keys.data(), sizeof(keys[ 0 ]) * order, current_addr);
    current_addr += sizeof(keys[ 0 ]) * order;

    // 写入 childrenOrValue 列表
    pager.write(childrenOrValue.data(), sizeof(childrenOrValue[ 0 ]) * order,
                current_addr);
    current_addr += sizeof(childrenOrValue[ 0 ]) * order;

    // 写入 _isLeaf
    pager.write(&(_isLeaf), sizeof(_isLeaf), current_addr);
    current_addr += sizeof(_isLeaf);

    // 写入 pre_leaf
    pager.write(&(pre_leaf), sizeof(pre_leaf), current_addr);
    current_addr += sizeof(pre_leaf);

    // 写入 next_leaf
    pager.write(&(next_leaf), sizeof(next_leaf), current_addr);
    current_addr += sizeof(next_leaf);
    return true;
}

bool BPlusTreeNode::isLeaf() const { return _isLeaf; }

bool BPlusTreeNode::can_add_entry() const { return len <= order - 2; }

bool BPlusTreeNode::insert_entry(std::int32_t key, std::uint32_t value) {
    auto pos =
        std::lower_bound(keys.begin(), keys.begin() + len, key) - keys.begin();
    if (pos < order && keys[ pos ] == key) {
        spdlog::error("already exists key: {}", key);
        return false;
    }

    keys.insert(keys.begin() + pos, key);
    childrenOrValue.insert(childrenOrValue.begin() + pos, value);
    len++;
    dump();
    return true;
}

std::uint32_t BPlusTreeNode::get_entry(std::int32_t key) {
    auto pos =
        std::lower_bound(keys.begin(), keys.begin() + len, key) - keys.begin();
    if (pos < len && keys[ pos ] == key) { return childrenOrValue[ pos ]; }
    return 0;
}

} // namespace minidb::storage