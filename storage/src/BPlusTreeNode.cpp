#include "BPlusTreeNode.h"
#include "Pager.h"

namespace minidb::storage {

BPlusTreeNode::BPlusTreeNode()
    : parent(0)
    , keys(order)
    , childrenOrValue(order)
    , _isLeaf(true)
    , nextLeaf(0)
    , len(0) {}

bool BPlusTreeNode::loads(std::uint32_t addr, Pager &pager) {
    std::uint32_t currentAddr = addr;

    pager.read(currentAddr, (char *) &(_isLeaf), sizeof(_isLeaf));
    currentAddr += sizeof(_isLeaf);

    // 读入父节点地址
    pager.read(currentAddr, (char *) &(parent), sizeof(parent));
    currentAddr += sizeof(parent);

    // 读入 key 列表
    pager.read(currentAddr, (char *) keys.data(), sizeof(keys[ 0 ]) * order);
    currentAddr += sizeof(keys[ 0 ]) * order;

    // 读入 childrenOrValue 列表
    pager.read(currentAddr, (char *) childrenOrValue.data(),
               sizeof(childrenOrValue[ 0 ]) * order);
    currentAddr += sizeof(childrenOrValue[ 0 ]) * order;

    return true;
}

} // namespace minidb::storage