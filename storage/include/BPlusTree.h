#pragma once

#include <cstddef>
#include <cstdint>
#include <utility>

namespace minidb::storage {

class BPlusTree {
public:
    BPlusTree(bool inMemory);

    bool insert(std::int32_t key, char *data, std::size_t dataSize);
    bool remove(std::int32_t key);
    std::pair<char *, std::int32_t> search(std::int32_t key);

    const int order = 512;
};
} // namespace minidb::storage