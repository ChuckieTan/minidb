#pragma once

#include <cstdint>
#include <new>

namespace minidb::operate {

class SQLBinaryData {
public:
    char *       data;
    std::int32_t size;
};

const std::int8_t DATA_INT    = 0;
const std::int8_t DATA_FLOAT  = 1;
const std::int8_t DATA_STRING = 2;
} // namespace minidb::operate