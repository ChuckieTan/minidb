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
    , currentNode(new BPlusTreeNode())
    , currentNodeAddr(0)
    , root(_root) {
    if (_root == 0) {
        // 如果树为空
        spdlog::info("root node doesn't exist, create a new root node");

        auto addr = createNode();
        changeRoot(addr);
        currentNode->parent   = 0;
        currentNode->_isLeaf  = true;
        currentNode->nextLeaf = 0;
        currentNodeAddr       = addr;
        dumpNode();
    } else {
        loadNode(_root);
        currentNodeAddr = _root;
    }
}

BPlusTree::~BPlusTree() { delete currentNode; }

bool BPlusTree::loadNode(std::uint32_t addr) {
    std::uint32_t currentAddr = addr;

    // 读入父节点地址
    pager.read(currentAddr, (char *) &(currentNode->parent),
               sizeof(currentNode->parent));
    currentAddr += sizeof(currentNode->parent);

    // 读入元素数量
    pager.read(currentAddr, (char *) &(currentNode->len),
               sizeof(currentNode->len));
    currentAddr += sizeof(currentNode->len);

    // 读入 key 列表
    pager.read(currentAddr, (char *) currentNode->keys.data(),
               sizeof(currentNode->keys[ 0 ]) * order);
    currentAddr += sizeof(currentNode->keys[ 0 ]) * order;

    // 读入 childrenOrValue 列表
    pager.read(currentAddr, (char *) currentNode->childrenOrValue.data(),
               sizeof(currentNode->childrenOrValue[ 0 ]) * order);
    currentAddr += sizeof(currentNode->childrenOrValue[ 0 ]) * order;

    // 读入 _isLeaf
    pager.read(currentAddr, (char *) &(currentNode->_isLeaf),
               sizeof(currentNode->_isLeaf));
    currentAddr += sizeof(currentNode->_isLeaf);

    // 读入 nextLeaf
    pager.read(currentAddr, (char *) &(currentNode->nextLeaf),
               sizeof(currentNode->nextLeaf));
    currentAddr += sizeof(currentNode->nextLeaf);

    currentNodeAddr = addr;
    return true;
}

bool BPlusTree::dumpNode() {
    std::uint32_t currentAddr = currentNodeAddr;

    // 写入父节点地址
    pager.write((char *) &(currentNode->parent), sizeof(currentNode->parent),
                currentAddr);
    currentAddr += sizeof(currentNode->parent);

    // 写入元素数量
    pager.write((char *) &(currentNode->len), sizeof(currentNode->len),
                currentAddr);
    currentAddr += sizeof(currentNode->len);

    // 写入 key 列表
    pager.write((char *) currentNode->keys.data(),
                sizeof(currentNode->keys[ 0 ]) * order, currentAddr);
    currentAddr += sizeof(currentNode->keys[ 0 ]) * order;

    // 写入 childrenOrValue 列表
    pager.write((char *) currentNode->childrenOrValue.data(),
                sizeof(currentNode->childrenOrValue[ 0 ]) * order, currentAddr);
    currentAddr += sizeof(currentNode->childrenOrValue[ 0 ]) * order;

    // 写入 _isLeaf
    pager.write((char *) &(currentNode->_isLeaf), sizeof(currentNode->_isLeaf),
                currentAddr);
    currentAddr += sizeof(currentNode->_isLeaf);

    // 写入 nextLeaf
    pager.write((char *) &(currentNode->nextLeaf),
                sizeof(currentNode->nextLeaf), currentAddr);
    currentAddr += sizeof(currentNode->nextLeaf);
    return true;
}

bool BPlusTree::searchNode(std::int32_t key) {
    loadNode(root);
    while (!currentNode->_isLeaf) {
        auto addr = std::lower_bound(currentNode->keys.begin(),
                                     currentNode->keys.end(), key) -
                    currentNode->keys.begin();
        loadNode(addr);
    }
    return true;
}

operate::SQLBinaryData BPlusTree::search(std::int32_t key) {
    searchNode(key);
    auto pos = std::lower_bound(currentNode->keys.begin(),
                                currentNode->keys.end(), key) -
               currentNode->keys.begin();
    char *        data = nullptr;
    std::uint32_t size = 0;
    if (pos < currentNode->order && currentNode->keys[ pos ] == key) {
        auto addr = currentNode->childrenOrValue[ pos ];
        size      = pager.getRowSize(addr);
        data      = new char[ size ];
        pager.readRow(addr, data);
    } else {
        spdlog::info("do not exists key: {}", key);
    }
    return operate::SQLBinaryData{ data, size };
}

bool BPlusTree::insert(std::int32_t key, char *data, std::size_t dataSize) {
    searchNode(key);
    auto pos = std::lower_bound(currentNode->keys.begin(),
                                currentNode->keys.end(), key) -
               currentNode->keys.begin();
    if (pos < order && currentNode->keys[ pos ]) {
        auto addr = pager.writeRow(data, dataSize);
        currentNode->keys.insert(currentNode->keys.begin() + pos, key);
        currentNode->childrenOrValue.insert(
            currentNode->childrenOrValue.begin() + pos, addr);
        currentNode->len++;
    }
}

std::uint32_t BPlusTree::createNode() {
    auto data = new char[ 4096 ];
    auto addr = pager.write_back(data, sizeof(data));
    delete[] data;
    return addr;
}

} // namespace minidb::storage