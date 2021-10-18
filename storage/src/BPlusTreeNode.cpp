#include "BPlusTreeNode.h"
#include "Pager.h"

namespace minidb::storage {

BPlusTreeNode::BPlusTreeNode(Pager &_pager)
    : parent(0)
    , keys(order)
    , childrenOrValue(order)
    , _isLeaf(true)
    , nextLeaf(0)
    , len(0)
    , pager(_pager) {}

bool BPlusTreeNode::load(std::uint32_t _addr) {
    std::uint32_t currentAddr = _addr;

    // 读入父节点地址
    pager.read({ (char *) &(parent), sizeof(parent) }, currentAddr);
    currentAddr += sizeof(parent);

    // 读入元素数量
    pager.read({ (char *) &(len), sizeof(len) }, currentAddr);
    currentAddr += sizeof(len);

    // 读入 key 列表
    pager.read({ (char *) keys.data(), sizeof(keys[ 0 ]) * order },
               currentAddr);
    currentAddr += sizeof(keys[ 0 ]) * order;

    // 读入 childrenOrValue 列表
    pager.read({ (char *) childrenOrValue.data(),
                 sizeof(childrenOrValue[ 0 ]) * order },
               currentAddr);
    currentAddr += sizeof(childrenOrValue[ 0 ]) * order;

    // 读入 _isLeaf
    pager.read({ (char *) &(_isLeaf), sizeof(_isLeaf) }, currentAddr);
    currentAddr += sizeof(_isLeaf);

    // 读入 nextLeaf
    pager.read({ (char *) &(nextLeaf), sizeof(nextLeaf) }, currentAddr);
    currentAddr += sizeof(nextLeaf);

    addr = _addr;
    return true;
}

bool BPlusTreeNode::dump(std::uint32_t _addr) {
    std::uint32_t currentAddr;
    if (_addr == 0) {
        // 保存到当前所存的位置
        currentAddr = addr;
    } else {
        // 保存到新位置
        currentAddr = _addr;
        addr        = _addr;
    }

    // 写入父节点地址
    pager.write({ (char *) &(parent), sizeof(parent) }, currentAddr);
    currentAddr += sizeof(parent);

    // 写入元素数量
    pager.write({ (char *) &(len), sizeof(len) }, currentAddr
                // namespace minidb::storage
    );
    currentAddr += sizeof(len);

    // 写入 key 列表
    pager.write({ (char *) keys.data(), sizeof(keys[ 0 ]) * order },
                currentAddr);
    currentAddr += sizeof(keys[ 0 ]) * order;

    // 写入 childrenOrValue 列表
    pager.write({ (char *) childrenOrValue.data(),
                  sizeof(childrenOrValue[ 0 ]) * order },
                currentAddr);
    currentAddr += sizeof(childrenOrValue[ 0 ]) * order;

    // 写入 _isLeaf
    pager.write({ (char *) &(_isLeaf), sizeof(_isLeaf) }, currentAddr);
    currentAddr += sizeof(_isLeaf);

    // 写入 nextLeaf
    pager.write({ (char *) &(nextLeaf), sizeof(nextLeaf) }, currentAddr);
    currentAddr += sizeof(nextLeaf);
    return true;
}

} // namespace minidb::storage