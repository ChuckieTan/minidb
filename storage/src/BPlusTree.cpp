#include "BPlusTree.h"
#include "BPlusTreeNode.h"
#include "Pager.h"
#include "SQLBinaryData.h"
#include "Storage.h"
#include "spdlog/spdlog.h"
#include <algorithm>
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

        auto _root_addr = createNode();

        change_root(_root_addr);
        change_first_leaf(_root_addr);
        change_last_leaf(_root_addr);

        current_node->parent    = 0;
        current_node->_isLeaf   = true;
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
    while (!current_node->_isLeaf) {
        auto index = std::lower_bound(current_node->keys.begin(),
                                      current_node->keys.end(), key) -
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
    if (current_node->can_add_entry()) {
        res = current_node->insert_entry(key, addr);
    } else {
        res = current_node->insert_entry(key, addr);
        res = res && split_leaf();
    }
    return res;
}

bool BPlusTree::split_leaf() {
    // 如果当前节点是根节点，那需要新建一个根节点作为分裂后节点的父节点
    if (current_node->addr == root_addr) {
        auto new_root     = new BPlusTreeNode(pager);
        new_root->parent  = 0;
        new_root->_isLeaf = false;
        new_root->len     = 0;
        new_root->dump(true);
        current_node->parent = new_root->addr;
        change_root(new_root->addr);
        delete new_root;
    }
    auto parent      = current_node->parent;
    auto new_node    = new BPlusTreeNode(pager);
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
    new_node->_isLeaf = true;

    // 获取 new_node 的磁盘地址
    new_node->addr = createNode();

    // 设置 pre_leaf 和 next_leaf
    new_node->pre_leaf  = current_node->addr;
    new_node->next_leaf = current_node->next_leaf;

    // 如果后面还有 leaf 的话，得设置后面 leaf 的 pre_leaf 指针
    if (current_node->next_leaf != 0) {
        auto next_next_leaf = new BPlusTreeNode(pager);
        next_next_leaf->load(current_node->next_leaf);

        next_next_leaf->pre_leaf = new_node->addr;
        next_next_leaf->dump();

        delete next_next_leaf;
    }
    current_node->next_leaf = new_node->addr;

    current_node->dump();
    new_node->dump();

    current_node->load(parent);
    //* currentNode 现在为 parent
    bool res = false;
    if (current_node->can_add_entry()) {
        res = current_node->insert_entry(new_node->keys[ 0 ], new_node->addr);
    } else {
        res = current_node->insert_entry(new_node->keys[ 0 ], new_node->addr);
        // 分裂父节点
        res = res && split_parent();
    }
    return res;
}

bool BPlusTree::split_parent() {
    // 如果当前节点是根节点，那需要新建一个根节点作为分裂后节点的父节点
    if (current_node->addr == root_addr) {
        auto new_root     = new BPlusTreeNode(pager);
        new_root->parent  = 0;
        new_root->_isLeaf = false;
        new_root->len     = 0;
        new_root->dump();
        current_node->parent = new_root->addr;
        change_root(new_root->addr);
        delete new_root;
    }
    auto parent      = current_node->parent;
    auto new_node    = new BPlusTreeNode(pager);
    new_node->parent = parent;
    new_node->addr   = createNode();

    // 复制一半元素
    for (int i = order / 2; i < current_node->len; i++) {
        new_node->keys[ i - order / 2 ] = current_node->keys[ i ];
        new_node->children_or_value[ i - order / 2 ] =
            current_node->children_or_value[ i ];
    }
    current_node->len = order / 2;
    new_node->len     = order - order / 2;

    new_node->_isLeaf = false;

    current_node->dump();
    new_node->dump();

    // 当前节点不是叶子节点，还需要更新新节点的子节点的父节点
    for (int i = 0; i < new_node->len; i++) {
        auto addr = new_node->children_or_value[ i ];
        pager.write_index_file(&new_node->addr, sizeof(new_node->addr), addr);
    }

    // 往父节点插入新节点的第一个元素
    current_node->load(parent);
    //* currentNode 现在为 parent
    if (current_node->can_add_entry()) {
        current_node->insert_entry(new_node->keys[ 0 ], new_node->addr);
    } else {
        current_node->insert_entry(new_node->keys[ 0 ], new_node->addr);
        // 分裂父节点
        split_parent();
    }
    return true;
}

std::uint64_t BPlusTree::createNode() {
    auto data = new char[ 4096 ];
    auto addr = pager.write_back(data, 4096);
    delete[] data;
    return addr;
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