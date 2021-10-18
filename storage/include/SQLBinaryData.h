#pragma once

#include <cstdint>
#include <new>

namespace minidb::storage {

class SQLBinaryData {
public:
    char *        data;
    std::uint32_t size;
};

} // namespace minidb::operate