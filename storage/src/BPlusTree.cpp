#include "BPlusTree.h"
#include "BPlusTreeNode.h"
#include "Pager.h"
#include "SQLBinaryData.h"
#include "Storage.h"
#include "spdlog/spdlog.h"
#include <algorithm>
#include <cstdint>
#include <string>
#include <utility>
namespace minidb::storage {

BPlusTree::BPlusTree(std::uint32_t _root, Pager &_pager, Storage &_storage,
                     const std::string &_table_name)
    : currentNode(new BPlusTreeNode(_pager))
    , root_addr(_root)
    , pager(_pager)
    , storage(_storage)
    , table_name(_table_name) {
    if (_root == 0) {
        // 如果树为空
        spdlog::info(
            "table: {} root node doesn't exist, create a new root node",
            table_name);

        auto _root_addr = createNode();

        change_root(_root_addr);
        change_first_leaf(_root_addr);
        change_last_leaf(_root_addr);

        currentNode->parent    = 0;
        currentNode->_isLeaf   = true;
        currentNode->next_leaf = 0;
        currentNode->dump(_root_addr);
    } else {
        currentNode->load(_root);
    }
}

bool BPlusTree::search_in_tree(std::int32_t key) {
    currentNode->load(root_addr);
    while (!currentNode->_isLeaf) {
        auto index = std::lower_bound(currentNode->keys.begin(),
                                      currentNode->keys.end(), key) -
                     currentNode->keys.begin();
        auto addr = currentNode->childrenOrValue[ index ];
        currentNode->load(addr);
    }
    return true;
}

storage::SQLBinaryData BPlusTree::search(std::int32_t key) {
    search_in_tree(key);

    auto addr = currentNode->get_entry(key);

    storage::SQLBinaryData data(0);
    if (addr != 0) {
        data = pager.read_row(addr);
    } else {
        spdlog::info("doesn't exists key: {}", key);
    }
    return data;
}

bool BPlusTree::insert(std::int32_t key, const SQLBinaryData &data) {
    bool res = false;

    search_in_tree(key);
    auto addr = pager.write_row(data.data.get(), data.size);
    if (currentNode->can_add_entry()) {
        res = currentNode->insert_entry(key, addr);
    } else {
        res = currentNode->insert_entry(key, addr);
        res = res && split_leaf();
    }
    return res;
}

bool BPlusTree::split_leaf() {
    // 如果当前节点是根节点，那需要新建一个根节点作为分裂后节点的父节点
    if (currentNode->addr == root_addr) {
        auto new_root     = new BPlusTreeNode(pager);
        new_root->parent  = 0;
        new_root->_isLeaf = false;
        new_root->len     = 0;
        new_root->dump();
        currentNode->parent = new_root->addr;
        change_root(new_root->addr);
        delete new_root;
    }
    auto parent      = currentNode->parent;
    auto new_node    = new BPlusTreeNode(pager);
    new_node->parent = parent;

    // 复制一半元素
    for (int i = order / 2; i < currentNode->len; i++) {
        new_node->keys[ i - order / 2 ] = currentNode->keys[ i ];
        new_node->childrenOrValue[ i - order / 2 ] =
            currentNode->childrenOrValue[ i ];
    }
    new_node->len    = currentNode->len - order;
    currentNode->len = order / 2;

    // 分裂出来的节点还是叶子节点
    new_node->_isLeaf = true;

    // 设置 pre_leaf 和 next_leaf
    new_node->pre_leaf  = currentNode->addr;
    new_node->next_leaf = currentNode->next_leaf;

    // new_node的磁盘地址得 dump 后才知道
    new_node->dump();
    currentNode->next_leaf = new_node->addr;

    // 如果后面还有 leaf 的话，得设置后面 leaf 的 pre_leaf 指针
    if (currentNode->next_leaf != 0) {
        auto next_next_leaf = new BPlusTreeNode(pager);
        next_next_leaf->load(currentNode->next_leaf);

        next_next_leaf->pre_leaf = new_node->addr;
        next_next_leaf->dump();

        delete next_next_leaf;
    }
    currentNode->next_leaf = new_node->addr;

    currentNode->dump();

    currentNode->load(parent);
    //* currentNode 现在为 parent
    bool res = false;
    if (currentNode->can_add_entry()) {
        res = currentNode->insert_entry(new_node->keys[ 0 ], new_node->addr);
    } else {
        res = currentNode->insert_entry(new_node->keys[ 0 ], new_node->addr);
        // 分裂父节点
        res = res && split_parent();
    }
    return res;
}

bool BPlusTree::split_parent() {
    // 如果当前节点是根节点，那需要新建一个根节点作为分裂后节点的父节点
    if (currentNode->addr == root_addr) {
        auto new_root     = new BPlusTreeNode(pager);
        new_root->parent  = 0;
        new_root->_isLeaf = false;
        new_root->len     = 0;
        new_root->dump();
        currentNode->parent = new_root->addr;
        change_root(new_root->addr);
        delete new_root;
    }
    auto parent      = currentNode->parent;
    auto new_node    = new BPlusTreeNode(pager);
    new_node->parent = parent;

    // 复制一半元素
    for (int i = order / 2; i < currentNode->len; i++) {
        new_node->keys[ i - order / 2 ] = currentNode->keys[ i ];
        new_node->childrenOrValue[ i - order / 2 ] =
            currentNode->childrenOrValue[ i ];
    }
    new_node->len    = currentNode->len - order;
    currentNode->len = order / 2;

    new_node->_isLeaf = false;

    currentNode->dump();
    new_node->dump();

    // 当前节点不是叶子节点，还需要更新新节点的子节点的父节点
    for (int i = 0; i < new_node->len; i++) {
        auto addr = new_node->childrenOrValue[ i ];
        pager.write_index_file(&new_node->addr, sizeof(new_node->addr), addr);
    }

    // 往父节点插入新节点的第一个元素
    currentNode->load(parent);
    //* currentNode 现在为 parent
    if (currentNode->can_add_entry()) {
        currentNode->insert_entry(new_node->keys[ 0 ], new_node->addr);
    } else {
        currentNode->insert_entry(new_node->keys[ 0 ], new_node->addr);
        // 分裂父节点
        split_parent();
    }
    return true;
}

std::uint32_t BPlusTree::createNode() {
    auto data = new char[ 4096 ];
    auto addr = pager.write_back(data, 4096);
    delete[] data;
    return addr;
}

bool BPlusTree::change_root(std::uint32_t addr) {
    auto &table_info       = storage.table_info_map[ table_name ];
    auto  root_define_addr = table_info.table_root_define_addr;
    table_info.root_addr   = addr;
    pager.write_index_file(&addr, sizeof(addr), root_define_addr);
    root_addr = addr;
    return true;
}

bool BPlusTree::change_first_leaf(std::uint32_t addr) {
    auto &table_info             = storage.table_info_map[ table_name ];
    auto  first_leaf_define_addr = table_info.first_leaf_define_addr;
    table_info.first_leaf_addr   = addr;
    pager.write_index_file(&addr, sizeof(addr), first_leaf_define_addr);
    first_leaf_addr = addr;
    return true;
}

bool BPlusTree::change_last_leaf(std::uint32_t addr) {
    auto &table_info            = storage.table_info_map[ table_name ];
    auto  last_leaf_define_addr = table_info.last_leaf_define_addr;
    table_info.last_leaf_addr   = addr;
    pager.write_index_file(&addr, sizeof(addr), last_leaf_define_addr);
    last_leaf_addr = addr;
    return true;
}

} // namespace minidb::storage