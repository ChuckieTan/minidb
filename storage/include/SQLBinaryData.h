#pragma once

#include <cstdint>
#include <memory>
#include <new>

namespace minidb::storage {

class SQLBinaryData {
public:
    std::unique_ptr<char[]> data;
    std::uint64_t           size;

    SQLBinaryData(std::uint64_t _size)
        : data(new char[_size])
        , size(_size) {}
};

} // namespace minidb::storage