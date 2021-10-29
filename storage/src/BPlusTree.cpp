#include "BPlusTree.h"
#include "BPlusTreeNode.h"
#include "Pager.h"
#include "SQLBinaryData.h"
#include "Storage.h"
#include "spdlog/spdlog.h"
#include <algorithm>
#include <climits>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
namespace minidb::storage {

BPlusTree::BPlusTree(std::uint64_t _root, std::uint64_t _first_leaf_addr,
                     std::uint64_t _last_leaf_addr, Pager &_pager,
                     Storage &_storage, const std::string &_table_name)
    : current_node(new BPlusTreeNode(_pager))
    , root_addr(_root)
    , first_leaf_addr(_first_leaf_addr)
    , last_leaf_addr(_last_leaf_addr)
    , pager(_pager)
    , storage(_storage)
    , table_name(_table_name)
    , _end(_pager) {
    if (_root == 0) {
        // 如果树为空
        spdlog::info(
            "table: {} root node doesn't exist, create a new root node",
            table_name);

        auto _root_addr = pager.new_page();

        change_root(_root_addr);
        change_first_leaf(_root_addr);
        change_last_leaf(_root_addr);

        current_node->parent    = 0;
        current_node->_is_leaf  = true;
        current_node->next_leaf = 0;
        current_node->dump(_root_addr);
    } else {
        current_node->load(_root);
    }

    _end.current_node->load(last_leaf_addr);
    _end.current_record_offset = _end.current_node->len;
}

bool BPlusTree::search_in_tree(std::int64_t key) {
    current_node->load(root_addr);
    while (!current_node->_is_leaf) {
        auto index = std::upper_bound(
                         current_node->keys.begin(),
                         current_node->keys.begin() + current_node->len, key) -
                     current_node->keys.begin();
        auto addr = current_node->children_or_value[ index ];
        current_node->load(addr);
    }
    return true;
}

storage::SQLBinaryData BPlusTree::search(std::int64_t key) {
    search_in_tree(key);

    auto addr = current_node->get_entry(key);

    storage::SQLBinaryData data(0);
    if (addr != 0) {
        data = pager.read_row(addr);
    } else {
        spdlog::info("doesn't exists key: {}", key);
    }
    return data;
}

bool BPlusTree::insert(std::int64_t key, const SQLBinaryData &data) {
    bool res = false;

    search_in_tree(key);
    auto addr = pager.write_row(data.data.get(), data.size);
    res       = current_node->insert_entry(key, addr);
    if (current_node->need_split()) { res = res && split_leaf(); }
    return res;
}

bool BPlusTree::split_leaf() {
    // 如果当前节点是根节点，那需要新建一个根节点作为分裂后节点的父节点
    if (current_node->addr == root_addr) {
        auto new_root      = std::make_unique<BPlusTreeNode>(pager);
        new_root->parent   = 0;
        new_root->len      = 0;
        new_root->_is_leaf = false;
        new_root->addr     = pager.new_page();
        new_root->children_or_value[ 0 ] = current_node->addr;
        current_node->parent             = new_root->addr;
        new_root->dump();
        change_root(new_root->addr);
        change_first_leaf(current_node->addr);
    }
    auto parent      = current_node->parent;
    auto new_node    = std::make_unique<BPlusTreeNode>(pager);
    new_node->parent = parent;

    // 复制一半元素
    for (int i = order / 2; i < current_node->len; i++) {
        new_node->keys[ i - order / 2 ] = current_node->keys[ i ];
        new_node->children_or_value[ i - order / 2 ] =
            current_node->children_or_value[ i ];
    }
    current_node->len = order / 2;
    new_node->len     = order - order / 2;

    // 分裂出来的节点还是叶子节点
    new_node->_is_leaf = true;

    // 获取 new_node 的磁盘地址
    new_node->addr = pager.new_page();

    // 设置 pre_leaf 和 next_leaf
    new_node->pre_leaf  = current_node->addr;
    new_node->next_leaf = current_node->next_leaf;

    // 如果后面还有 leaf 的话，得设置后面 leaf 的 pre_leaf 指针
    if (current_node->next_leaf != 0) {
        auto next_next_leaf = std::make_unique<BPlusTreeNode>(pager);
        next_next_leaf->load(current_node->next_leaf);

        next_next_leaf->pre_leaf = new_node->addr;
        next_next_leaf->dump();
    }
    current_node->next_leaf = new_node->addr;

    current_node->dump();
    new_node->dump();

    if (current_node->addr == last_leaf_addr) {
        change_last_leaf(new_node->addr);
    }

    current_node->load(parent);
    //* currentNode 现在为 parent
    bool res = current_node->insert_entry(new_node->keys[ 0 ], new_node->addr);
    if (current_node->need_split()) {
        // 分裂父节点
        res = res && split_parent();
    }
    return res;
}

bool BPlusTree::split_parent() {
    // 如果当前节点是根节点，那需要新建一个根节点作为分裂后节点的父节点
    if (current_node->addr == root_addr) {
        auto new_root      = std::make_unique<BPlusTreeNode>(pager);
        new_root->parent   = 0;
        new_root->_is_leaf = false;
        new_root->len      = 0;
        new_root->addr     = pager.new_page();
        new_root->children_or_value[ 0 ] = current_node->addr;
        current_node->parent             = new_root->addr;
        new_root->dump();
        change_root(new_root->addr);
    }
    auto parent      = current_node->parent;
    auto new_node    = std::make_unique<BPlusTreeNode>(pager);
    new_node->parent = parent;
    new_node->addr   = pager.new_page();

    // 复制一半元素
    for (int i = order / 2; i < current_node->len; i++) {
        new_node->keys[ i - order / 2 ] = current_node->keys[ i ];
        new_node->children_or_value[ i - order / 2 ] =
            current_node->children_or_value[ i ];
    }
    // 对于非叶子节点，children 比 key 多一
    new_node->children_or_value[ current_node->len - order / 2 ] =
        current_node->children_or_value[ current_node->len ];
    // current_node 需要上升一个记录到父节点
    current_node->len = order / 2 - 1;
    new_node->len     = order - order / 2;

    new_node->_is_leaf = false;

    current_node->dump();
    new_node->dump();

    // 当前节点不是叶子节点，还需要更新新节点的子节点的父节点
    for (int i = 0; i < new_node->len + 1; i++) {
        auto addr = new_node->children_or_value[ i ];
        pager.write_index_file(&new_node->addr, sizeof(new_node->addr), addr);
    }

    // 旧节点的最后一个元素上升到父节点
    auto k = current_node->keys[ current_node->len ];
    auto v = new_node->addr;
    current_node->load(parent);
    //* currentNode 现在为 parent
    bool res = current_node->insert_entry(k, v);
    if (current_node->need_split()) {
        // 分裂父节点
        split_parent();
    }
    return res;
}

bool BPlusTree::remove(std::int64_t key) {
    bool res = false;
    search_in_tree(key);
    res = current_node->remove_entry(key);
    return res;
}

bool BPlusTree::change_root(std::uint64_t addr) {
    auto &table_info       = storage.table_info_map[ table_name ];
    auto  root_define_addr = table_info.table_root_define_addr;
    table_info.root_addr   = addr;
    pager.write_index_file(&addr, sizeof(addr), root_define_addr);
    root_addr = addr;
    return true;
}

bool BPlusTree::change_first_leaf(std::uint64_t addr) {
    auto &table_info             = storage.table_info_map[ table_name ];
    auto  first_leaf_define_addr = table_info.first_leaf_define_addr;
    table_info.first_leaf_addr   = addr;
    pager.write_index_file(&addr, sizeof(addr), first_leaf_define_addr);
    first_leaf_addr = addr;
    return true;
}

bool BPlusTree::change_last_leaf(std::uint64_t addr) {
    auto &table_info            = storage.table_info_map[ table_name ];
    auto  last_leaf_define_addr = table_info.last_leaf_define_addr;
    table_info.last_leaf_addr   = addr;
    pager.write_index_file(&addr, sizeof(addr), last_leaf_define_addr);
    last_leaf_addr = addr;
    return true;
}

BPlusTree::iterator::BPlusTreeIterator(Pager &pager)
    : current_record_offset(0)
    , current_node(new BPlusTreeNode(pager)) {}

BPlusTree::iterator::BPlusTreeIterator(const BPlusTreeIterator &it) {
    current_node          = it.current_node;
    current_record_offset = it.current_record_offset;
}

SQLBinaryData BPlusTree::iterator::operator*() {
    return current_node->pager.read_row(
        current_node->children_or_value[ current_record_offset ]);
}

BPlusTree::iterator &BPlusTree::iterator::operator++() {
    do {
        // 如果当前节点是最后一个节点，则可以一直加到 len，此时迭代器为 end
        if (current_node->next_leaf == 0) {
            if (current_record_offset == current_node->len) {
                return *this;
            } else {
                current_record_offset++;
            }
        } else {
            // 如果不是最后一个节点，处于最后一个记录时加载下一个节点
            if (current_record_offset == current_node->len - 1) {
                current_node->load(current_node->next_leaf);
                current_record_offset = 0;
            } else {
                current_record_offset++;
            }
        }
    } while (current_node->children_or_value[ current_record_offset ] ==
             ULLONG_MAX);
    return *this;
}

bool BPlusTree::iterator::operator!=(const BPlusTreeIterator &that) {
    return !(current_node->addr == that.current_node->addr &&
             current_record_offset == that.current_record_offset);
}

BPlusTree::iterator BPlusTree::begin() {
    iterator iter(pager);
    iter.current_node->load(first_leaf_addr);
    iter.current_record_offset = 0;
    return iter;
}

BPlusTree::iterator &BPlusTree::end() {
    if (_end.current_node->addr != last_leaf_addr) {
        _end.current_node->load(last_leaf_addr);
    }
    _end.current_record_offset = _end.current_node->len;
    return _end;
}

} // namespace minidb::storage