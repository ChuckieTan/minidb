#pragma once

#include <cstdint>
#include <new>

namespace minidb::storage {

class SQLBinaryData {
public:
    char *        data;
    std::uint32_t size;
};

const std::int8_t DATA_INT    = 0;
const std::int8_t DATA_FLOAT  = 1;
const std::int8_t DATA_STRING = 2;
} // namespace minidb::operate