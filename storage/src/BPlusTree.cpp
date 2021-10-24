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

        auto addr = createNode();
        changeRoot(addr);
        currentNode->parent   = 0;
        currentNode->_isLeaf  = true;
        currentNode->nextLeaf = 0;
        currentNode->dump(addr);
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

    auto                   addr = currentNode->get_entry(key);
    storage::SQLBinaryData data{ nullptr, 0 };
    if (addr != 0) {
        data = pager.readRow(addr);
    } else {
        spdlog::info("doesn't exists key: {}", key);
    }
    return data;
}

bool BPlusTree::insert(std::int32_t key, SQLBinaryData data) {
    bool res = false;

    search_in_tree(key);
    auto addr = pager.writeRow(data);
    if (currentNode->can_add_entry()) {
        res = currentNode->insert_entry(key, addr);
    } else {
        currentNode->insert_entry(key, addr);
        res = split_node();
    }
    return res;
}

bool BPlusTree::split_node() {
    if (currentNode->isLeaf() && currentNode->addr == root_addr) {
        auto new_root     = new BPlusTreeNode(pager);
        new_root->parent  = 0;
        new_root->_isLeaf = false;
        new_root->len     = 0;
        new_root->dump();
        currentNode->parent = new_root->addr;
        changeRoot(new_root->addr);
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

    new_node->_isLeaf = true;

    // 下一个叶子节点
    new_node->nextLeaf = currentNode->nextLeaf;
    // 新节点的磁盘地址得 dump 后才知道
    new_node->dump();
    currentNode->nextLeaf = new_node->addr;
    currentNode->dump();

    // 如果当前节点不是叶子节点，还需要更新新节点的子节点的父节点
    if (!currentNode->isLeaf()) {
        for (int i = 0; i < new_node->len; i++) {
            auto addr = new_node->childrenOrValue[ i ];
            pager.write({ (char *) &new_node->addr, sizeof(new_node->addr) },
                        addr);
        }
    }
    currentNode->load(parent);
    //* currentNode 现在为 parent
    bool res = false;
    if (currentNode->can_add_entry()) {
        res = currentNode->insert_entry(new_node->keys[ 0 ], new_node->addr);
    } else {
        res = currentNode->insert_entry(new_node->keys[ 0 ], new_node->addr);
        // 分裂父节点
        res = res && split_node();
    }
    return res;
}

std::uint32_t BPlusTree::createNode() {
    auto data = new char[ 4096 ];
    auto addr = pager.write_back({ data, 4096 });
    delete[] data;
    return addr;
}

bool BPlusTree::changeRoot(std::uint32_t addr) {
    auto &table_info       = storage.table_info_map[ table_name ];
    auto  root_define_addr = table_info.table_root_define_addr;
    table_info.root_addr   = addr;
    pager.write({ (char *) &addr, sizeof(addr) }, root_define_addr);
    root_addr = addr;
    return true;
}

} // namespace minidb::storage