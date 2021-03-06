#include "BPlusTreeNode.h"
#include "Pager.h"

#include <algorithm>
#include <climits>
#include <spdlog/spdlog.h>

namespace minidb::storage {

BPlusTreeNode::BPlusTreeNode(Pager &_pager)
    : parent(0)
    , keys(order)
    , children_or_value(order + 1)
    , _is_leaf(true)
    , pre_leaf(0)
    , next_leaf(0)
    , len(0)
    , pager(_pager) {}

bool BPlusTreeNode::load(std::uint64_t _addr) {
    std::uint64_t current_addr = _addr;

    // 读入父节点地址
    pager.read_index_file(&(parent), sizeof(parent), current_addr);
    current_addr += sizeof(parent);

    // 读入元素数量
    pager.read_index_file(&(len), sizeof(len), current_addr);
    current_addr += sizeof(len);

    // 读入 key 列表
    pager.read_index_file(keys.data(), sizeof(keys[ 0 ]) * order, current_addr);
    current_addr += sizeof(keys[ 0 ]) * order;

    // 读入 childrenOrValue 列表
    pager.read_index_file(children_or_value.data(),
                          sizeof(children_or_value[ 0 ]) * order, current_addr);
    current_addr += sizeof(children_or_value[ 0 ]) * order;

    // 读入 _isLeaf
    pager.read_index_file(&(_is_leaf), sizeof(_is_leaf), current_addr);
    current_addr += sizeof(_is_leaf);

    // 读入 pre_leaf
    pager.read_index_file(&(pre_leaf), sizeof(pre_leaf), current_addr);
    current_addr += sizeof(pre_leaf);

    // 读入 next_leaf
    pager.read_index_file(&(next_leaf), sizeof(next_leaf), current_addr);
    current_addr += sizeof(next_leaf);

    addr = _addr;
    return true;
}

bool BPlusTreeNode::dump(std::uint64_t _addr) {
    std::uint64_t current_addr;
    if (_addr == 0) {
        // 保存到当前所存的位置
        current_addr = addr;
    } else {
        // 保存到新位置
        current_addr = _addr;
        addr         = _addr;
    }

    // 写入父节点地址
    pager.write_index_file(&(parent), sizeof(parent), current_addr);
    current_addr += sizeof(parent);

    // 写入元素数量
    pager.write_index_file(&(len), sizeof(len), current_addr
                           // namespace minidb::storage
    );
    current_addr += sizeof(len);

    // 写入 key 列表
    pager.write_index_file(keys.data(), sizeof(keys[ 0 ]) * order,
                           current_addr);
    current_addr += sizeof(keys[ 0 ]) * order;

    // 写入 childrenOrValue 列表
    pager.write_index_file(children_or_value.data(),
                           sizeof(children_or_value[ 0 ]) * order,
                           current_addr);
    current_addr += sizeof(children_or_value[ 0 ]) * order;

    // 写入 _isLeaf
    pager.write_index_file(&(_is_leaf), sizeof(_is_leaf), current_addr);
    current_addr += sizeof(_is_leaf);

    // 写入 pre_leaf
    pager.write_index_file(&(pre_leaf), sizeof(pre_leaf), current_addr);
    current_addr += sizeof(pre_leaf);

    // 写入 next_leaf
    pager.write_index_file(&(next_leaf), sizeof(next_leaf), current_addr);
    current_addr += sizeof(next_leaf);
    return true;
}

bool BPlusTreeNode::is_leaf() const { return _is_leaf; }

bool BPlusTreeNode::need_split() const { return len > order - 1; }

bool BPlusTreeNode::insert_entry(std::int64_t key, std::uint64_t value) {
    auto pos =
        std::lower_bound(keys.begin(), keys.begin() + len, key) - keys.begin();
    if (pos < order && keys[ pos ] == key &&
        children_or_value[ pos ] != ULLONG_MAX) {
        spdlog::error("already exists key: {}", key);
        return false;
    }
    if (pos < order && keys[ pos ] == key &&
        children_or_value[ pos ] == ULLONG_MAX) {
        children_or_value[ pos ] = value;
    } else {
        keys.insert(keys.begin() + pos, key);
        if (is_leaf()) {
            children_or_value.insert(children_or_value.begin() + pos, value);
        } else {
            children_or_value.insert(children_or_value.begin() + pos + 1,
                                     value);
        }
        len++;
    }
    dump();
    return true;
}

std::uint64_t BPlusTreeNode::get_entry(std::int64_t key) {
    auto pos =
        std::lower_bound(keys.begin(), keys.begin() + len, key) - keys.begin();
    if (pos < len && keys[ pos ] == key) { return children_or_value[ pos ]; }
    return 0;
}

bool BPlusTreeNode::can_remove_entry() const { return len >= order / 2; }

bool BPlusTreeNode::remove_entry(std::int64_t key) {
    auto pos =
        std::lower_bound(keys.begin(), keys.begin() + len, key) - keys.begin();
    if (pos < len && keys[ pos ] == key) {
        children_or_value[ pos ] = ULLONG_MAX;
        dump();
        return true;
    } else {
        return false;
    }
}

} // namespace minidb::storage