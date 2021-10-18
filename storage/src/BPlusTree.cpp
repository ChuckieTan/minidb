#include "BPlusTree.h"
#include "BPlusTreeNode.h"
#include "Pager.h"
#include "SQLBinaryData.h"
#include "spdlog/spdlog.h"
#include <algorithm>
#include <cstdint>
#include <string>
#include <utility>
namespace minidb::storage {

BPlusTree::BPlusTree(const std::string &_fileName, bool _isInMemory,
                     std::uint32_t _root)
    : pager(_fileName, _isInMemory)
    , currentNode(new BPlusTreeNode(pager))
    , root(_root) {
    if (_root == 0) {
        // 如果树为空
        spdlog::info("root node doesn't exist, create a new root node");

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

BPlusTree::~BPlusTree() { delete currentNode; }

bool BPlusTree::search_in_tree(std::int32_t key) {
    currentNode->load(root);
    while (!currentNode->_isLeaf) {
        auto addr = std::lower_bound(currentNode->keys.begin(),
                                     currentNode->keys.end(), key) -
                    currentNode->keys.begin();
        currentNode->load(addr);
    }
    return true;
}

storage::SQLBinaryData BPlusTree::search(std::int32_t key) {
    search_in_tree(key);

    auto addr = currentNode->get_entry(key);
    storage::SQLBinaryData data{ nullptr, 0 };
    if (addr != 0) {
        data = pager.readRow(addr);
    } else {
        spdlog::info("doesn't exists key: {}", key);
    }
    return data;
}

bool BPlusTree::insert(std::int32_t key, SQLBinaryData) {
    search_in_tree(key);
    auto pos = std::lower_bound(currentNode->keys.begin(),
                                currentNode->keys.end(), key) -
               currentNode->keys.begin();
    if (pos < order && currentNode->keys[ pos ] == key) {
        spdlog::error("already exists key: {}", key);
        return false;
    }
    auto addr = pager.writeRow(data, dataSize);
    currentNode->keys.insert(currentNode->keys.begin() + pos, key);
    currentNode->childrenOrValue.insert(
        currentNode->childrenOrValue.begin() + pos, addr);
    currentNode->len++;
}

std::uint32_t BPlusTree::createNode() {
    auto data = new char[ 4096 ];
    auto addr = pager.write_back(data, sizeof(data));
    delete[] data;
    return addr;
}

} // namespace minidb::storage